#include "flipperzero.h"

#include <QThread>
#include <QSerialPort>

#include "recoverycontroller.h"
#include "storagecontroller.h"
#include "remotecontroller.h"

#include "macros.h"

namespace Flipper {

using namespace Zero;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),

    m_isPersistent(false),
    m_isOnline(true),
    m_isError(false),

    m_deviceInfo(info),

    m_progress(0),
    m_remote(nullptr),
    m_recovery(nullptr),
    m_storage(nullptr)
{
    initControllers();
}

FlipperZero::~FlipperZero()
{
    setOnline(false);
}

void FlipperZero::reset(const Zero::DeviceInfo &info)
{
    setDeviceInfo(info);
    initControllers();

    setError(QStringLiteral("No error"), false);
    setProgress(0);
    setOnline(true);
}

void FlipperZero::setDeviceInfo(const Zero::DeviceInfo &info)
{
    // Not checking the huge structure for equality
    m_deviceInfo = info;
    emit deviceInfoChanged();
}

void FlipperZero::setPersistent(bool set)
{
    if(set == m_isPersistent) {
        return;
    }

    m_isPersistent = set;
    emit isPersistentChanged();
}

void FlipperZero::setOnline(bool set)
{
    if(set == m_isOnline) {
        return;
    }

    m_isOnline = set;
    emit isOnlineChanged();
}

void FlipperZero::setError(const QString &msg, bool set)
{
    m_isError = set;

    if(!msg.isEmpty()) {
        error_msg(msg);
        m_errorString = msg;
    }

    emit isErrorChanged();
}

bool FlipperZero::isPersistent() const
{
    return m_isPersistent;
}

bool FlipperZero::isOnline() const
{
    return m_isOnline;
}

bool FlipperZero::isError() const
{
    return m_isError;
}

bool FlipperZero::bootToDFU()
{
    setMessage("Entering DFU bootloader mode...");

    auto *serialPort = new QSerialPort(m_deviceInfo.serialInfo, this);

    const auto success = serialPort->open(QIODevice::WriteOnly) && serialPort->setDataTerminalReady(true) &&
                        (serialPort->write(QByteArrayLiteral("\rdfu\r\n")) > 0) && serialPort->waitForBytesWritten(1000);
    if(!success) {
        setError("Can't detach the device: Failed to reset in DFU mode");
        error_msg(QString("Serial port status: %1").arg(serialPort->errorString()));
    }

    serialPort->close();
    serialPort->deleteLater();

    return success;
}

const QString &FlipperZero::name() const
{
    return m_deviceInfo.name;
}

const QString &FlipperZero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &FlipperZero::target() const
{
    return m_deviceInfo.target;
}

const QString &FlipperZero::version() const
{
    return m_deviceInfo.firmware.version;
}

const QString &FlipperZero::messageString() const
{
    return m_statusMessage;
}

const QString &FlipperZero::errorString() const
{
    return m_errorString;
}

double FlipperZero::progress() const
{
    return m_progress;
}

const DeviceInfo &FlipperZero::deviceInfo() const
{
    return m_deviceInfo;
}

bool FlipperZero::isDFU() const
{
    return m_deviceInfo.usbInfo.productID() == 0xdf11;
}

Flipper::Zero::RemoteController *FlipperZero::remote() const
{
    return m_remote;
}

RecoveryController *FlipperZero::recovery() const
{
    return m_recovery;
}

StorageController *FlipperZero::storage() const
{
    return m_storage;
}

void FlipperZero::setName(const QString &name)
{
    if(m_deviceInfo.name == name) {
        return;
    }

    m_deviceInfo.name = name;
    emit deviceInfoChanged();
}

void FlipperZero::setTarget(const QString &target)
{
    if(m_deviceInfo.target == target) {
        return;
    }

    m_deviceInfo.target = target;
    emit deviceInfoChanged();
}

void FlipperZero::setVersion(const QString &version)
{
    if(m_deviceInfo.firmware.version == version) {
        return;
    }

    m_deviceInfo.firmware.version = version;
    emit deviceInfoChanged();
}

void FlipperZero::setMessage(const QString &message)
{
    info_msg(message);
    m_statusMessage = message;
    emit messageChanged();
}

void FlipperZero::setProgress(double progress)
{
    if(qFuzzyCompare(m_progress, progress)) {
        return;
    }

    m_progress = progress;
    emit progressChanged();
}

void FlipperZero::initControllers()
{
    if(m_remote) {
       m_remote->deleteLater();
       m_remote = nullptr;
    }

    if(m_recovery) {
       m_recovery->deleteLater();
       m_recovery = nullptr;
    }

    if(m_storage) {
       m_storage->deleteLater();
       m_storage = nullptr;
    }

    // TODO: better message delivery system
    if(isDFU()) {
        m_recovery = new RecoveryController(m_deviceInfo.usbInfo, this);
        connect(m_recovery, &RecoveryController::messageChanged, this, [=]() {
            setMessage(m_recovery->message());
        });

        connect(m_recovery, &RecoveryController::errorOccured, this, [=]() {
            setError(m_recovery->errorString());
        });

        connect(m_recovery, &RecoveryController::progressChanged, this, [=]() {
            setProgress(m_recovery->progress());
        });

    } else {
        m_remote = new RemoteController(m_deviceInfo.serialInfo, this);
        m_storage = new StorageController(m_deviceInfo.serialInfo, this);
    }
}

}
