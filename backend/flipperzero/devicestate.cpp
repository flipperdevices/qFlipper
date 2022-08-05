#include "devicestate.h"

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

using namespace Flipper;
using namespace Zero;

DeviceState::DeviceState(const DeviceInfo &deviceInfo, QObject *parent):
    QObject(parent),
    m_deviceInfo(deviceInfo),
    m_isPersistent(false),
    m_isOnline(false),
    m_isAllowVirtualDisplay(true),
    m_error(BackendError::NoError),
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
        qCDebug(CATEGORY_DEBUG) << "Received a DeviceInfo too early, queueing it...";
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

void DeviceState::setStorageInfo(const StorageInfo &newStorageInfo)
{
    m_deviceInfo.storage = newStorageInfo;
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
    return m_error != BackendError::NoError;
}

bool DeviceState::isRecoveryMode() const
{
    return m_deviceInfo.usbInfo.productID() == 0xdf11;
}

bool DeviceState::isAllowVirtualDisplay() const
{
    return m_isAllowVirtualDisplay;
}

void DeviceState::setAllowVirtualDisplay(bool set)
{
    if(m_isAllowVirtualDisplay == set) {
        return;
    }

    m_isAllowVirtualDisplay = set;
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

BackendError::ErrorType DeviceState::error() const
{
    return m_error;
}

void DeviceState::setError(BackendError::ErrorType error, const QString &errorString)
{
    m_error = error;
    m_errorString = errorString;

    emit isErrorChanged();
}

void DeviceState::clearError()
{
    m_error = BackendError::NoError;
    m_errorString.clear();

    emit isErrorChanged();
}

const QString &DeviceState::name() const
{
    return m_deviceInfo.name;
}

void DeviceState::onDeviceInfoChanged()
{
    clearError();
    setProgress(-1.0);
}

void DeviceState::onIsOnlineChanged()
{
    processQueue();
}

void DeviceState::processQueue()
{
    if(!m_isOnline && !m_queue.isEmpty()) {
        qCDebug(CATEGORY_DEBUG) << "Took the latest DeviceInfo from the queue";
        setDeviceInfo(m_queue.takeLast());
        m_queue.clear();
    }
}
