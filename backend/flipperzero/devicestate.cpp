#include "devicestate.h"

#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

DeviceState::DeviceState(const DeviceInfo &deviceInfo, QObject *parent):
    QObject(parent),
    m_deviceInfo(deviceInfo),
    m_isPersistent(false),
    m_isOnline(true),
    m_isError(false),
    m_progress(-1.0)
{
    if(m_deviceInfo.serialPort) {
        m_deviceInfo.serialPort->setParent(this);
    }
}

const DeviceInfo &DeviceState::deviceInfo() const
{
    return m_deviceInfo;
}

void DeviceState::setDeviceInfo(const DeviceInfo &newDeviceInfo)
{
    if(m_deviceInfo.serialPort) {
       m_deviceInfo.serialPort->deleteLater();
    }

    m_deviceInfo = newDeviceInfo;

    if(m_deviceInfo.serialPort) {
        m_deviceInfo.serialPort->setParent(this);
    }

    emit deviceInfoChanged();

    setError(false);
    setProgress(-1.0);
    setOnline(true);
}

bool DeviceState::isPersistent() const
{
    return m_isPersistent;
}

void DeviceState::setPersistent(bool set)
{
    if(m_isPersistent == set) {
        return;
    }

    m_isPersistent = set;
    emit isPersistentChanged();
}

bool DeviceState::isOnline() const
{
    return m_isOnline;
}

void DeviceState::setOnline(bool set)
{
    if(m_isOnline == set) {
        return;
    }

    m_isOnline = set;
    emit isOnlineChanged();
}

bool DeviceState::isError() const
{
    return m_isError;
}

void DeviceState::setError(bool set)
{
    if(m_isError == set) {
        return;
    }

    m_isError = set;
    emit errorChanged();
}

bool DeviceState::isRecoveryMode() const
{
    return m_deviceInfo.usbInfo.productID() == 0xdf11;
}

double DeviceState::progress() const
{
    return m_progress;
}

void DeviceState::setProgress(double newProgress)
{
    if(qFuzzyCompare(m_progress, newProgress)) {
        return;
    }

    m_progress = newProgress;
    emit progressChanged();
}

const QString &DeviceState::statusString() const
{
    return m_statusString;
}

void DeviceState::setStatusString(const QString &newStatusString)
{
    if(m_statusString == newStatusString) {
        return;
    }

    m_statusString = newStatusString;
    emit statusChanged();
}

const QString &DeviceState::errorString() const
{
    return m_errorString;
}

void DeviceState::setErrorString(const QString &newErrorString)
{
    if(m_errorString == newErrorString) {
        return;
    }

    m_errorString = newErrorString;
    m_isError = true;

    emit errorChanged();
}

const QString &DeviceState::name() const
{
    return m_deviceInfo.name;
}
