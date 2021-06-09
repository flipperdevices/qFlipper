#ifndef USBBACKEND_H
#define USBBACKEND_H

#ifdef USB_BACKEND_LIBUSB
#include "backends/libusbbackend.h"
#include "backends/libusbdevicedetector.h"
#else
#error "No USB backend confugured"
#endif

#endif // USBBACKEND_H
