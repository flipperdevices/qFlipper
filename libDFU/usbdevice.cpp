#include "usbdevice.h"

#include <exception>
#include <QDebug>

USBDevice::USBDevice(const USBDeviceParams &info, QObject *parent):
    QObject(parent)
{
    backend().initDevice(&m_handle, info);
}

USBDevice::~USBDevice()
{
    if(m_isOpen) {
        close();
    }
}

bool USBDevice::open()
{
    if(!m_isOpen) {
        m_isOpen = backend().openDevice(m_handle);
    }

    return m_isOpen;
}

void USBDevice::close()
{
    if(!m_isOpen) {
        return;
    }

    backend().closeDevice(m_handle);
    m_isOpen = false;
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
