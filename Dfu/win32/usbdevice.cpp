#include "usbdevice.h"

#include <windows.h>
#include <setupapi.h>
#include <winusb.h>

#include "macros.h"

struct USBDevice::USBDevicePrivate
{
    QByteArray filePath;
    HANDLE fileHandle = nullptr;
    WINUSB_INTERFACE_HANDLE deviceHandle = nullptr;
};

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_p(new USBDevicePrivate)
{
}

USBDevice::~USBDevice()
{
    delete m_p;
}

bool USBDevice::open()
{
    return false;
}

void USBDevice::close()
{
}

bool USBDevice::claimInterface(int interfaceNum)
{
    return false;
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    return false;
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    return false;
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    return false;
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    QByteArray buf(length, 0);
    return buf;
}

QByteArray USBDevice::extraInterfaceDescriptor(int interfaceNum, uint8_t type, int length)
{
    QByteArray ret;
    return ret;
}

QByteArray USBDevice::stringInterfaceDescriptor(int interfaceNum)
{
    const auto BUF_SIZE = 254;
    QByteArray buf(BUF_SIZE, 0);

    return buf;
}
