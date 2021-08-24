#ifndef USBDEVICE_H
#define USBDEVICE_H

namespace USBRequest {

enum EndpointDirection {
    ENDPOINT_IN = 0x80,
    ENDPOINT_OUT = 0x00,
};

enum RequestType {
    REQUEST_TYPE_STANDARD = 0x00,
    REQUEST_TYPE_CLASS = (0x01 << 5),
    REQUEST_TYPE_VENDOR = (0x02 << 5),
    REQUEST_TYPE_RESERVED = (0x03 << 5)
};

enum RequestRecipient {
    RECIPIENT_DEVICE = 0x00,
    RECIPIENT_INTERFACE = 0x01,
    RECIPIENT_ENDPOINT = 0x02,
    RECIPIENT_OTHER = 0x03
};

}

#ifdef USB_BACKEND_LIBUSB
#include "libusb/usbdevice.h"
#include "libusb/usbdevicedetector.h"

#elif defined(USB_BACKEND_WIN32)
#include "win32/usbdevice.h"
#include "win32/usbdevicedetector.h"

#else
#error "No USB backend confugured"
#endif

#endif // USBDEVICE_H
