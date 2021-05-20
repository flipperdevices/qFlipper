#include "usbdevice.h"

#include <exception>
#include <QDebug>

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_location(info.data())
{
    if(!backend().findDevice(&m_handle, m_location)) {
        qCritical() << "Failed to find specified device";
        return;
    }
}

USBDevice::~USBDevice()
{
    close();
    backend().unrefDevice(m_handle);
}

bool USBDevice::open()
{
    return backend().openDevice(m_handle);
}

void USBDevice::close()
{
    backend().closeDevice(m_handle);
}

bool USBDevice::claimInterface(int interfaceNum)
{
    return backend().claimInterface(m_handle, interfaceNum);
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    return backend().releaseInterface(m_handle, interfaceNum);
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    return backend().setInterfaceAltSetting(m_handle, interfaceNum, alt);
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data)
{
    return backend().controlTransfer(m_handle, requestType, request, value, index, data);
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    return backend().controlTransfer(m_handle, requestType, request, value, index, length);
}

QByteArray USBDevice::extraInterfaceDescriptor()
{
    return backend().getExtraInterfaceDescriptor(m_handle);
}

QByteArray USBDevice::stringInterfaceDescriptor(int interfaceNum)
{
    return backend().getStringInterfaceDescriptor(m_handle, interfaceNum);
}

USBBackend &USBDevice::backend()
{
    static USBBackend backendInstance;
    return backendInstance;
}
