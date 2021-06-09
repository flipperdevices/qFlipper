#ifndef USBDEVICE_H
#define USBDEVICE_H

#ifdef USB_BACKEND_LIBUSB
#include "libusb/usbdevice.h"
#include "libusb/usbdevicedetector.h"

#else
#error "No USB backend confugured"
#endif

#endif // USBDEVICE_H
