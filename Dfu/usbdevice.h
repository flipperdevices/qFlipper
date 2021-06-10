#ifndef USBDEVICE_H
#define USBDEVICE_H

#ifdef USB_BACKEND_LIBUSB
#include "libusb/usbdevice.h"
#include "libusb/usbdevicedetector.h"

#elif USB_BACKEND_WIN32
#include "win32/usbdevice.h"
#include "win32/usbdevicedetector.h"

#else
#error "No USB backend confugured"
#endif

#endif // USBDEVICE_H
