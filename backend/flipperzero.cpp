#include "flipperzero.h"

#include <QDebug>
#include <QBuffer>
#include <QIODevice>
#include <QSerialPort>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzeroremote.h"
#include "serialhelper.h"
#include "dfusedevice.h"
#include "dfusefile.h"
#include "macros.h"
#include <QThread>

static const auto STARTUP_MESSAGE = QObject::tr("Probing");
static const auto UPDATE_MESSAGE = QObject::tr("Update");
static const auto ERROR_MESSAGE = QObject::tr("Error");

using namespace Flipper;
 Zero::Zero(const USBDeviceInfo &parameters, QObject *parent):
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
        QSerialPortInfo info;
        uint32_t toolate_count = 200;
        while (--toolate_count) {
            info = SerialHelper::findSerialPort(parameters.serialNumber());
            QThread::usleep(50);
            if(!info.isNull()) {
                break;
            }
        }

        if(info.isNull()) {
            setStatusMessage(ERROR_MESSAGE);
            return;
        }

        m_port = new QSerialPort(info, this);
        m_remote = new ZeroRemote(m_port, this);

        fetchInfoVCPMode();
    }
}

bool Zero::detach()
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

bool Zero::download(QIODevice *file)
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

const QString &Zero::name() const
{
    return m_name;
}

const QString &Zero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &Zero::target() const
{
    return m_target;
}

const QString &Zero::version() const
{
    return m_version;
}

const QString &Zero::statusMessage() const
{
    return m_statusMessage;
}

double Zero::progress() const
{
    return m_progress;
}

const USBDeviceInfo &Zero::info() const
{
    return m_info;
}

bool Zero::isDFU() const
{
    return m_info.productID() == 0xdf11;
}

ZeroRemote *Zero::remote() const
{
    return m_remote;
}

void Zero::setName(const QString &name)
{
    if(m_name != name) {
        emit nameChanged(m_name = name);
    }
}

void Zero::setTarget(const QString &target)
{
    if(m_target != target) {
        emit targetChanged(m_target = target);
    }
}

void Zero::setVersion(const QString &version)
{
    if(m_version != version) {
        emit versionChanged(m_version = version);
    }
}

void Zero::setStatusMessage(const QString &message)
{
    if(m_statusMessage != message) {
        emit statusMessageChanged(m_statusMessage = message);
    }
}

void Zero::setProgress(double progress)
{
    if(m_progress != progress) {
        emit progressChanged(m_progress = progress);
    }
}

// Since we're not using threads anymore, rework it with signal-slot friendly approach
void Zero::fetchInfoVCPMode()
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

void Zero::fetchInfoDFUMode()
{
    QMutexLocker locker(&m_deviceMutex);

    // TODO: Error handling
    const uint32_t FLIPPER_OTP_ADDRESS = 0x1fff7000UL;
    const size_t FLIPPER_OTP_SIZE = 16, FLIPPER_TARGET_OFFSET = 1, FLIPPER_NAME_OFFSET = 8;
    const int FLIPPER_OTP_ALT_NUM = 2;

    QByteArray otpData;
    QBuffer otpDataBuf(&otpData);
    DfuseDevice dev(m_info);

    otpDataBuf.open(QIODevice::WriteOnly);

    const auto success = dev.beginTransaction() &&
                         dev.upload(&otpDataBuf, FLIPPER_OTP_ADDRESS, FLIPPER_OTP_SIZE, FLIPPER_OTP_ALT_NUM) &&
                         dev.endTransaction();

    otpDataBuf.close();

    check_return_void(success, "Failed to read OTP data");

    if(!otpData.isEmpty()) {
        setName(otpData.right(FLIPPER_NAME_OFFSET));
        setTarget("f" + QString::number(otpData.at(FLIPPER_TARGET_OFFSET)));
    }

    if(m_statusMessage == STARTUP_MESSAGE) {
        setStatusMessage(UPDATE_MESSAGE);
    }
}
