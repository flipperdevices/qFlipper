#include "recoverycontroller.h"

#include "device/stm32wb55.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

RecoveryController::RecoveryController(USBDeviceInfo info, QObject *parent):
    QObject(parent),
    m_usbInfo(info)
{}

RecoveryController::~RecoveryController()
{}

const USBDeviceInfo &RecoveryController::usbInfo() const
{
    return m_usbInfo;
}

void RecoveryController::setUSBInfo(const USBDeviceInfo &info)
{
    m_usbInfo = info;
}

void RecoveryController::setMessage(const QString &msg)
{
    m_message = msg;
    emit messageChanged();
}

void RecoveryController::setError(const QString &msg)
{
    m_message = msg;
    m_isError = true;
    emit errorOccured();
}
