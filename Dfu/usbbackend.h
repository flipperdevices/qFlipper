#ifndef USBBACKEND_H
#define USBBACKEND_H

#ifdef USB_BACKEND_LIBUSB
#include "backends/libusbbackend.h"
#else
#error "Only libusb backend is implemented right now"
#endif

#endif // USBBACKEND_H
