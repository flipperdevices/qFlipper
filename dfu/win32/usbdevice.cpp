#include "usbdevice.h"

#include <windows.h>
#include <setupapi.h>
#include <winusb.h>
#include <usbspec.h>

#include <QThread>

#include "debug.h"

#define RETRY_COUNT 5
#define RETRY_INTERVAL_MS 50

struct USBDevice::USBDevicePrivate
{
    HANDLE fileHandle;
    WINUSB_INTERFACE_HANDLE deviceHandle;
    WINUSB_INTERFACE_HANDLE interfaceHandle;
    QByteArray filePath;
};

USBDevice::USBDevice(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),
    m_p(new USBDevicePrivate)
{
    m_p->filePath = info.backendData().toByteArray();
}

USBDevice::~USBDevice()
{
    close();
    delete m_p;
}

bool USBDevice::open()
{
    check_return_val(!m_isOpen, "Device already open, ignoring call", true);

    m_p->fileHandle = CreateFileA(m_p->filePath.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);

    check_return_bool(m_p->fileHandle != INVALID_HANDLE_VALUE, "Failed to open file for USB device");

    const auto success = WinUsb_Initialize(m_p->fileHandle, &m_p->deviceHandle);
    m_isOpen = (success == TRUE);

    if(!m_isOpen) {
        CloseHandle(m_p->fileHandle);
        error_msg("Failed to initialise WinUsb device");
    }

    return m_isOpen;
}

void USBDevice::close()
{
    if(!m_isOpen) return;

    WinUsb_Free(m_p->deviceHandle);
    CloseHandle(m_p->fileHandle);

    m_isOpen = false;
}

bool USBDevice::claimInterface(int interfaceNum)
{
    Q_UNUSED(interfaceNum);
    return true;
}

bool USBDevice::releaseInterface(int interfaceNum)
{
    Q_UNUSED(interfaceNum);
    return true;
}

bool USBDevice::setInterfaceAltSetting(int interfaceNum, uint8_t alt)
{
    Q_UNUSED(interfaceNum);

    int retryCount = RETRY_COUNT;
    BOOL success;

    do {
        if((success = WinUsb_SetCurrentAlternateSetting(m_p->deviceHandle, alt)) == TRUE) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    check_return_bool(success == TRUE, "Failed to set alternate settiing");
    return true;
}

bool USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, const QByteArray &buf)
{
    Q_UNUSED(requestType)

    int retryCount = RETRY_COUNT;
    BOOL success;

    ULONG size;
    WINUSB_SETUP_PACKET sp;

    sp.RequestType = requestType;
    sp.Request = request;
    sp.Value = value;
    sp.Index = index;
    sp.Length = buf.size();

    do {
        if((success = WinUsb_ControlTransfer(m_p->deviceHandle, sp, (unsigned char*)buf.data(), buf.size(), &size, nullptr)) == TRUE) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    check_return_bool(success == TRUE, "Failed to perform control transfer");
    check_return_bool(size == buf.size(), "Requested and transferred data size differ");

    return true;
}

QByteArray USBDevice::controlTransfer(uint8_t requestType, uint8_t request, uint16_t value, uint16_t index, uint16_t length)
{
    Q_UNUSED(requestType)

    int retryCount = RETRY_COUNT;
    BOOL success;

    ULONG size;
    WINUSB_SETUP_PACKET sp;

    sp.RequestType = requestType;
    sp.Request = request;
    sp.Value = value;
    sp.Index = index;
    sp.Length = length;

    QByteArray buf(length, 0);
    do {
        if((success = WinUsb_ControlTransfer(m_p->deviceHandle, sp, (unsigned char*)buf.data(), length, &size, nullptr)) == TRUE) { break; }
        QThread::msleep(RETRY_INTERVAL_MS);
    } while(--retryCount);

    check_return_val(success == TRUE, "Failed to perform control transfer", QByteArray());
    if(success == FALSE) {
        buf.clear();
        error_msg("Failed to perform control transfer");

    } else if(size != length) {
       buf.resize(size);
       debug_msg("Requested and transferred data size differ");

    } else {}

    return buf;
}

QByteArray USBDevice::extraInterfaceDescriptor(int interfaceNum, uint8_t type, int length)
{
    Q_UNUSED(interfaceNum);

    QByteArray ret;
    ULONG sizeTransferred;

    const auto BUF_SIZE = 1024;
    unsigned char buf[BUF_SIZE];

    ((USB_CONFIGURATION_DESCRIPTOR*)buf)->bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE;

    const auto success = WinUsb_GetDescriptor(m_p->deviceHandle, USB_CONFIGURATION_DESCRIPTOR_TYPE, 0, 0, buf, BUF_SIZE, &sizeTransferred);
    check_return_val(success == TRUE, "Failed to retrieve configuration descriptor", ret);

    for(ULONG i = 0; i < sizeTransferred;) {
        auto *cd = (USB_COMMON_DESCRIPTOR*)(buf + i);
        if(cd->bDescriptorType == type && cd->bLength == length) {
            ret.append((char*)cd, cd->bLength);
            break;
        }

        i += cd->bLength;
    }

    return ret;
}

QByteArray USBDevice::stringInterfaceDescriptor(uint8_t alt)
{
    QByteArray ret;

    USB_INTERFACE_DESCRIPTOR ifd;
    ifd.bDescriptorType = USB_INTERFACE_DESCRIPTOR_TYPE;

    auto success = WinUsb_QueryInterfaceSettings(m_p->deviceHandle, alt, &ifd);
    check_return_val(success == TRUE, "Failed to get interface descriptor", ret);

    const auto BUF_SIZE = 1024;
    unsigned char buf[BUF_SIZE];

    ULONG sizeTransferred;
    success = WinUsb_GetDescriptor(m_p->deviceHandle, USB_STRING_DESCRIPTOR_TYPE, ifd.iInterface, 0, buf, BUF_SIZE, &sizeTransferred);
    check_return_val(success == TRUE, "Failed to get string interface descriptor", ret);

    auto *sd = (PUSB_STRING_DESCRIPTOR)buf;
    ret = QString::fromUtf16((ushort *)sd->bString).toLocal8Bit();

    return ret;
}
