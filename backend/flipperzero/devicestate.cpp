#include "devicestate.h"

#include <QSerialPort>

#include "helper/serialinithelper.h"

using namespace Flipper;
using namespace Zero;

DeviceState::DeviceState(const DeviceInfo &deviceInfo, QObject *parent):
    QObject(parent),
    m_deviceInfo(deviceInfo),
    m_serialPort(nullptr),
    m_isPersistent(false),
    m_isOnline(false),
    m_isError(false),
    m_progress(-1.0)
{
    connect(this, &DeviceState::deviceInfoChanged, this, &DeviceState::onDeviceInfoChanged);
    connect(this, &DeviceState::isOnlineChanged, this, &DeviceState::onIsOnlineChanged);

    onDeviceInfoChanged();
}

const DeviceInfo &DeviceState::deviceInfo() const
{
    return m_deviceInfo;
}

void DeviceState::setDeviceInfo(const DeviceInfo &newDeviceInfo)
{
    if(m_isOnline) {
        m_queue.enqueue(newDeviceInfo);
        return;
    }

    if(newDeviceInfo.usbInfo.productID() == 0xdf11) {
        // Keep most of the data from previous session
        m_deviceInfo.usbInfo = newDeviceInfo.usbInfo;
        m_deviceInfo.portInfo = newDeviceInfo.portInfo;

    } else {
        m_deviceInfo = newDeviceInfo;
    }

    emit deviceInfoChanged();
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
    emit isErrorChanged();
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
    emit statusStringChanged();
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

    emit isErrorChanged();
}

const QString &DeviceState::name() const
{
    return m_deviceInfo.name;
}

QSerialPort *DeviceState::serialPort() const
{
    return m_serialPort;
}

void DeviceState::onDeviceInfoChanged()
{
    setError(false);
    setProgress(-1.0);

    if(isRecoveryMode()) {
        setOnline(true);
        return;
    }

    createSerialPort();
}

void DeviceState::onIsOnlineChanged()
{
    deleteSerialPort();
    processQueue();
}

void DeviceState::createSerialPort()
{
    auto *helper = new SerialInitHelper(m_deviceInfo.portInfo, this);
    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        if(helper->isError()) {
            setErrorString(tr("Failed to initialize serial port"));

        } else {
            m_serialPort = helper->serialPort();
            setOnline(true);
        }

        helper->deleteLater();
    });
}

void DeviceState::deleteSerialPort()
{
    if(!m_isOnline && m_serialPort) {
        m_serialPort->deleteLater();
        m_serialPort = nullptr;
    }
}

void DeviceState::processQueue()
{
    if(!m_isOnline && !m_queue.isEmpty()) {
        setDeviceInfo(m_queue.takeLast());
        m_queue.clear();
    }
}
