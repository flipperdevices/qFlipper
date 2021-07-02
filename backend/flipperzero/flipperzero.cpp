#include "flipperzero.h"

#include <QDebug>
#include <QBuffer>
#include <QIODevice>
#include <QSerialPort>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentRun>

#include "remotecontroller.h"
#include "serialhelper.h"
#include "dfusedevice.h"
#include "factoryinfo.h"
#include "dfusefile.h"
#include "macros.h"

#include "device/stm32wb55.h"

// I will sort this out, I promise!
static const auto STARTUP_MESSAGE = QObject::tr("Probing");
static const auto UPDATE_MESSAGE = QObject::tr("Update");
static const auto ERROR_MESSAGE = QObject::tr("Error");

using namespace Flipper;

FlipperZero::FlipperZero(const USBDeviceInfo &parameters, QObject *parent):
    QObject(parent),

    m_info(parameters),

    m_name("N/A"),
    m_target("N/A"),
    m_version("N/A"),
    m_statusMessage(STARTUP_MESSAGE),
    m_progress(0),
    m_port(nullptr),
    m_remote(nullptr)
{
    if(isDFU()) {
        fetchInfoDFUMode();
    } else {
        const auto info = SerialHelper::findSerialPort(parameters.serialNumber());

        if(info.isNull()) {
            setStatusMessage(ERROR_MESSAGE);
            return;
        }

        m_port = new QSerialPort(info, this);
        m_remote = new Zero::RemoteController(m_port, this);

        fetchInfoVCPMode();
    }
}

bool FlipperZero::detach()
{
    const auto success = m_port->open(QIODevice::WriteOnly) && m_port->setDataTerminalReady(true) &&
                        (m_port->write("\rdfu\r") >= 0) && m_port->flush();

    if(!success) {
        error_msg("Failed to reset device to DFU mode");
        setStatusMessage(ERROR_MESSAGE);
    }

    m_port->close();

    return success;
}

bool FlipperZero::setBootMode(BootMode mode)
{
    STM32WB55::STM32WB55 device(m_info);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    auto ob = device.optionBytes();

    check_return_bool(ob.isValid(), "Failed to read option bytes");

    ob.setNBoot0(mode == BootMode::Normal);
    ob.setNSwBoot0(mode == BootMode::Normal);

    check_return_bool(device.setOptionBytes(ob), "Failed to set option bytes");
    // The device is going to reset itself here, not bothering to end the transaction

    return true;
}

bool FlipperZero::downloadFirmware(QIODevice *file)
{
    QMutexLocker locker(&m_deviceMutex);

    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open firmware file");
    check_return_bool(file->bytesAvailable(), "The firmware file is empty");

    setStatusMessage(tr("Updating"));

    DfuseFile fw(file);
    DfuseDevice dev(m_info);

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) &&
                         dev.leave() && dev.endTransaction();

    check_continue(success, "Failed to download the firmware");

    file->close();

    return success;
}

const QString &FlipperZero::name() const
{
    return m_name;
}

const QString &FlipperZero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &FlipperZero::target() const
{
    return m_target;
}

const QString &FlipperZero::version() const
{
    return m_version;
}

const QString &FlipperZero::statusMessage() const
{
    return m_statusMessage;
}

double FlipperZero::progress() const
{
    return m_progress;
}

const USBDeviceInfo &FlipperZero::info() const
{
    return m_info;
}

bool FlipperZero::isDFU() const
{
    return m_info.productID() == 0xdf11;
}

Flipper::Zero::RemoteController *FlipperZero::remote() const
{
    return m_remote;
}

void FlipperZero::setName(const QString &name)
{
    if(m_name != name) {
        emit nameChanged(m_name = name);
    }
}

void FlipperZero::setTarget(const QString &target)
{
    if(m_target != target) {
        emit targetChanged(m_target = target);
    }
}

void FlipperZero::setVersion(const QString &version)
{
    if(m_version != version) {
        emit versionChanged(m_version = version);
    }
}

void FlipperZero::setStatusMessage(const QString &message)
{
    if(m_statusMessage != message) {
        emit statusMessageChanged(m_statusMessage = message);
    }
}

void FlipperZero::setProgress(double progress)
{
    if(m_progress != progress) {
        emit progressChanged(m_progress = progress);
    }
}

// Since we're not using threads anymore, rework it with signal-slot friendly approach
void FlipperZero::fetchInfoVCPMode()
{
    if(!m_port->open(QIODevice::ReadWrite)) {
        // TODO: Error handling
        setStatusMessage(ERROR_MESSAGE);
        error_msg("Failed to open port");
        return;
    }

    static const auto getValue = [](const QByteArray &line) {
        const auto fields = line.split(':');

        if(fields.size() != 2) {
            return QByteArray();
        }

        return fields.last().trimmed();
    };

    m_port->setDataTerminalReady(true);
    m_port->write("\rdevice_info\r");
    m_port->flush();

    qint64 bytesAvailable;

    do {
        bytesAvailable = m_port->bytesAvailable();
        m_port->waitForReadyRead(50);
    } while(bytesAvailable != m_port->bytesAvailable());

    do {
        const auto line = m_port->readLine();

        if(line.startsWith("hardware_name")) {
            setName(getValue(line));
        } else if(line.startsWith("hardware_target")) {
            setTarget("f" + getValue(line));
        } else if(line.startsWith("firmware_version")) {
            setVersion(getValue(line));
        } else {}

    } while(m_port->canReadLine());

    m_port->close();
    setStatusMessage(UPDATE_MESSAGE);
}

void FlipperZero::fetchInfoDFUMode()
{
    STM32WB55::STM32WB55 device(m_info);

    check_return_void(device.beginTransaction(), "Failed to initiate transaction");
    const Flipper::Zero::FactoryInfo info(device.otpData(Flipper::Zero::FactoryInfo::size()));

    if(info.isValid()) {
        setTarget(QString("f%1").arg(info.target()));
        setName(info.name());
    }

    if(m_statusMessage == STARTUP_MESSAGE) {
        setStatusMessage(info.isValid() ? UPDATE_MESSAGE : ERROR_MESSAGE);
    }

//    auto opt = device.optionBytes();

//    qDebug() << "Before:"
//             << "nBOOT0:" << opt.nBoot0()
//             << "nBOOT1:" << opt.nBoot1()
//             << "nSWBOOT0:" << opt.nSwBoot0();

//    opt.setNBoot0(false);
//    opt.setNSwBoot0(false);

<<<<<<< HEAD:backend/flipperzero/flipperzero.cpp
//    qDebug() << "After:"
//             << "nBOOT0:" << opt.nBoot0()
//             << "nBOOT1:" << opt.nBoot1()
//             << "nSWBOOT0:" << opt.nSwBoot0();
=======
    if(!otpData.isEmpty()) {
        setName(otpData.right(FLIPPER_NAME_OFFSET));
        setTarget("f" + QString::number(otpData.at(FLIPPER_TARGET_OFFSET)));
    }
>>>>>>> master:backend/flipperzero.cpp

//    device.setOptionBytes(opt);

    check_return_void(device.endTransaction(), "Failed to end transaction");
}
