#include "usbdevice.h"

#include <QMutex>

#include "usbbackend.h"

USBDevice::USBDevice(const USBDeviceParams &parameters, QObject *parent):
    QObject(parent)
{
    backend().initDevice(&m_handle, parameters);
}

USBDevice::~USBDevice()
{
    if(m_isOpen) {
        close();
    }
}

bool USBDevice::open()
{
    backendMutex().lock();

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
    backendMutex().unlock();

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

USBDEviceDetector *USBDevice::detector()
{
    static USBDEviceDetector detector;
    return &detector;
}

USBBackend &USBDevice::backend()
{
    static USBBackend backendInstance;
    return backendInstance;
}

QMutex &USBDevice::backendMutex()
{
    static QMutex mutex;
    return mutex;
}

USBDEviceDetector::USBDEviceDetector(QObject *parent):
    QObject(parent)
{
    connect(&USBDevice::backend(), &USBBackend::devicePluggedIn, this, &USBDEviceDetector::devicePluggedIn);
    connect(&USBDevice::backend(), &USBBackend::deviceUnplugged, this, &USBDEviceDetector::deviceUnplugged);
}

void USBDEviceDetector::registerHotplugEvent(const QList<USBDeviceParams> &paramList)
{
    USBDevice::backend().registerHotplugEvent(paramList);
}
