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
static const auto DONE_MESSAGE = QObject::tr("Done");

using namespace Flipper;

FlipperZero::FlipperZero(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),

    m_isPersistent(false),
    m_isConnected(true),

    m_name("N/A"),
    m_target("N/A"),
    m_version("N/A"),
    m_statusMessage(STARTUP_MESSAGE),
    m_progress(0),

    m_remote(nullptr)
{
    setDeviceInfo(info);
}

void FlipperZero::setDeviceInfo(const USBDeviceInfo &info)
{
    setProgress(0);

    m_info = info;

    if(isDFU()) {
        if(m_remote) {
            m_remote->deleteLater();
            m_remote = nullptr;
        }

        fetchInfoDFUMode();

    } else {
        const auto portInfo = SerialHelper::findSerialPort(info.serialNumber());

        if(portInfo.isNull()) {
            setStatusMessage(ERROR_MESSAGE);
            return;
        }

        if(m_remote) {
            m_remote->deleteLater();
        }

        m_remote = new Zero::RemoteController(portInfo, this);

        fetchInfoVCPMode();
    }

    emit isDFUChanged();

    m_isConnected = true;
}

void FlipperZero::setPersistent(bool set)
{
    if(set == m_isPersistent) {
        return;
    }

    m_isPersistent = set;
}

void FlipperZero::setConnected(bool set)
{
    if(set == m_isConnected) {
        return;
    }

    m_isConnected = set;
}

bool FlipperZero::isPersistent() const
{
    return m_isPersistent;
}

bool FlipperZero::isConnected() const
{
    return m_isConnected;
}

bool FlipperZero::detach()
{
    QSerialPort port(SerialHelper::findSerialPort(m_info.serialNumber()));

    const auto success = port.open(QIODevice::WriteOnly) && port.setDataTerminalReady(true) &&
                        (port.write("\rdfu\r") >= 0) && port.flush();

    if(!success) {
        error_msg("Failed to reset device to DFU mode");
        setStatusMessage(ERROR_MESSAGE);
    } else {
        setConnected(false);
    }

    port.close();

    return success;
}

bool FlipperZero::setBootMode(BootMode mode)
{
    QMutexLocker locker(&m_deviceMutex);

    STM32WB55::STM32WB55 device(m_info);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    auto ob = device.optionBytes();

    check_return_bool(ob.isValid(), "Failed to read option bytes");

    ob.setNBoot0(mode == BootMode::Normal);
    ob.setNSwBoot0(mode == BootMode::Normal);

    const auto success = device.setOptionBytes(ob);
    check_return_bool(success, "Failed to set option bytes");

    setConnected(false);
    return true;
}

bool FlipperZero::startFUS()
{
    QMutexLocker locker(&m_deviceMutex);

    info_msg("Starting FUS...");
    return true;
}

bool FlipperZero::startWirelessStack()
{
    QMutexLocker locker(&m_deviceMutex);

    info_msg("Starting Wireless Stack...");
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

    setStatusMessage(success ? DONE_MESSAGE : ERROR_MESSAGE);
    setConnected(!success);

    return success;
}

bool FlipperZero::downloadWirelessStack(QIODevice *file, uint32_t addr)
{
    Q_UNUSED(file);
    Q_UNUSED(addr);
    QMutexLocker locker(&m_deviceMutex);

    info_msg("Downloading wireless stack...");

    return true;
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
    if(!qFuzzyCompare(m_progress, progress)) {
        emit progressChanged(m_progress = progress);
    }
}

void FlipperZero::fetchInfoVCPMode()
{
    QSerialPort port(SerialHelper::findSerialPort(m_info.serialNumber()));

    if(!port.open(QIODevice::ReadWrite)) {
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

    port.setDataTerminalReady(true);
    port.write("\rdevice_info\r");
    port.flush();

    qint64 bytesAvailable;

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    do {
        const auto line = port.readLine();

        if(line.startsWith("hardware_name")) {
            setName(getValue(line));
        } else if(line.startsWith("hardware_target")) {
            setTarget("f" + getValue(line));
        } else if(line.startsWith("firmware_version")) {
            setVersion(getValue(line));
        } else {}

    } while(port.canReadLine());

    port.close();
    setStatusMessage(UPDATE_MESSAGE);
}

void FlipperZero::fetchInfoDFUMode()
{
    QMutexLocker locker(&m_deviceMutex);

    STM32WB55::STM32WB55 device(m_info);

    check_return_void(device.beginTransaction(), "Failed to initiate transaction");
    const Flipper::Zero::FactoryInfo info(device.OTPData(Flipper::Zero::FactoryInfo::size()));
    check_return_void(device.endTransaction(), "Failed to end transaction");

    if(info.isValid()) {
        setTarget(QString("f%1").arg(info.target()));
        setName(info.name());
    }

    if(m_statusMessage == STARTUP_MESSAGE) {
        setStatusMessage(info.isValid() ? UPDATE_MESSAGE : ERROR_MESSAGE);
    }
}
