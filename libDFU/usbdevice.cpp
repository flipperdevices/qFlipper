#include "usbdevice.h"

#include <QDebug>

// TODO: Surround in conditional compilation (and adapt project file)
#include "libusbusbdevicebackend.h"

static constexpr const char *dbgLabel = "USBDevice:";

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_location(info.data()),
    m_backend(new LibusbUSBDeviceBackend(this))
{
    if(!m_backend->init()) {
        qCritical() << dbgLabel << "Failed to initialise USB backend";
        return;
    }

    if(!m_backend->findDevice(m_location)) {
        qCritical() << dbgLabel << "Failed to find specified device";
        return;
    }
}

USBDevice::~USBDevice()
{}

bool USBDevice::open()
{
    return m_backend->openDevice();
}

void USBDevice::close()
{
    m_backend->closeDevice();
}

void USBDevice::reenumerate()
{
// TODO: Not implemented
}

bool USBDevice::claimInterface(int interfaceNum)
{
    return m_backend->claimInterface(interfaceNum);
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    return m_backend->releaseInterface(interfaceNum);
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    return m_backend->setInterfaceAltSetting(interfaceNum, alt);
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &data)
{
    return m_backend->controlTransfer(requestType, request, value, index, data);
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    return m_backend->controlTransfer(requestType, request, value, index, length);
}

QByteArray USBDevice::extraInterfaceDescriptor()
{
    return m_backend->getExtraInterfaceDescriptor();
}

QByteArray USBDevice::stringInterfaceDescriptor(int interfaceNum)
{
    return m_backend->getStringInterfaceDescriptor(interfaceNum);
}
