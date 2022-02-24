#include "usbdevicedetector.h"

#include <libusb.h>
#include <QTimer>

#include "debug.h"

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent)
{
    libusb_init(nullptr);
}

USBDeviceDetector::~USBDeviceDetector()
{
    libusb_exit(nullptr);
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
    check_return_bool(libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG), "Hotplug support is expected, but is not present");

    for(const auto &info : wantedList) {
        const auto events = libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);
        const auto err = libusb_hotplug_register_callback(nullptr, events, LIBUSB_HOTPLUG_NO_FLAGS, info .vendorID(),
                                                          info .productID(), LIBUSB_HOTPLUG_MATCH_ANY, libusbHotplugCallback, this, nullptr);

        check_return_bool(!err, "Failed to register hotplug callback");
    }

    return startTimer(100);
}

void USBDeviceDetector::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);

    struct timeval timeout = {0, 10000};
    libusb_handle_events_timeout_completed(nullptr, &timeout, nullptr);
}

static USBDeviceInfo getDeviceInfo(const USBDeviceInfo &info)
{
    auto *dev = (libusb_device*)info.backendData().value<void*>();

    libusb_device_descriptor desc;
    check_return_val(!libusb_get_device_descriptor(dev, &desc),"Failed to get device descriptor", USBDeviceInfo());

    struct libusb_device_handle *handle;
    check_return_val(!libusb_open(dev, &handle), "Failed to open device", USBDeviceInfo());

    USBDeviceInfo newinfo = info;
    unsigned char buf[0xff];

    check_return_val(libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, sizeof(buf)) >= 0, "Failed to get manufacturer string descriptor", USBDeviceInfo());
    newinfo.setManufacturer(QString::fromLocal8Bit((const char*)buf));

    check_return_val(libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf)) >= 0,"Failed to get product string descriptor", USBDeviceInfo());
    newinfo.setProductDescription(QString::fromLocal8Bit((const char*)buf));

    check_return_val(libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, sizeof(buf)) >= 0, "Failed to get device serial number", USBDeviceInfo());
    newinfo.setSerialNumber(QString::fromLocal8Bit((const char*)buf));

    libusb_close(handle);
    return newinfo;
}

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    Q_UNUSED(ctx)

    auto *detector = (USBDeviceDetector*)user_data;

    libusb_device_descriptor desc;
    check_return_val(!libusb_get_device_descriptor(dev, &desc),"Failed to get device descriptor", 0);

    const auto info = USBDeviceInfo(desc.idVendor, desc.idProduct).withBackendData(QVariant::fromValue((void*)dev));

    if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        // Get string descriptors out of callback context
        QTimer::singleShot(0, detector, [=]() {
            emit detector->devicePluggedIn(getDeviceInfo(info));
        });

    } else if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        emit detector->deviceUnplugged(info);
    } else {
        debug_msg("Unhandled libusb event");
    }

    return 0;
}
