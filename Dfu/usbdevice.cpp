#include "usbdevice.h"

#include <QMutex>

#include "usbbackend.h"

USBDevice::USBDevice(const USBDeviceParams &parameters, QObject *parent):
    QObject(parent)
{
    USBBackend::instance()->initDevice(&m_handle, parameters);
}

USBDevice::~USBDevice()
{
    if(m_isOpen) {
        close();
    }
}

bool USBDevice::open()
{
//    backendMutex().lock();

    if(!m_isOpen) {
        m_isOpen = USBBackend::instance()->openDevice(m_handle);
    }

    return m_isOpen;
}

void USBDevice::close()
{
    if(!m_isOpen) {
        return;
    }

    USBBackend::instance()->closeDevice(m_handle);
//    backendMutex().unlock();

    m_isOpen = false;
}

bool USBDevice::claimInterface(int interfaceNum)
{
    return USBBackend::instance()->claimInterface(m_handle, interfaceNum);
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    return USBBackend::instance()->releaseInterface(m_handle, interfaceNum);
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    return USBBackend::instance()->setInterfaceAltSetting(m_handle, interfaceNum, alt);
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data)
{
    return USBBackend::instance()->controlTransfer(m_handle, requestType, request, value, index, data);
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    return USBBackend::instance()->controlTransfer(m_handle, requestType, request, value, index, length);
}

QByteArray USBDevice::extraInterfaceDescriptor()
{
    return USBBackend::instance()->getExtraInterfaceDescriptor(m_handle);
}

QByteArray USBDevice::stringInterfaceDescriptor(int interfaceNum)
{
    return USBBackend::instance()->getStringInterfaceDescriptor(m_handle, interfaceNum);
}

QMutex *USBDevice::backendMutex()
{
    static QMutex mutex;
    return &mutex;
}
