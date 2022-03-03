#include "usbdevicedetector.h"

#include <libusb.h>

#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent),
    m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &USBDeviceDetector::processEvents);
    libusb_init(nullptr);
}

USBDeviceDetector::~USBDeviceDetector()
{
    libusb_exit(nullptr);
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
    if(!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        qCDebug(CATEGORY_DEBUG) << "Failed to get device descriptor";
        return false;
    }

    m_timer->stop();

    for(const auto &info : wantedList) {
        const auto events = libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);
        const auto err = libusb_hotplug_register_callback(nullptr, events, LIBUSB_HOTPLUG_ENUMERATE, info.vendorID(),
                                                          info.productID(), LIBUSB_HOTPLUG_MATCH_ANY, libusbHotplugCallback, this, nullptr);
        if(err) {
            qCDebug(CATEGORY_DEBUG) << "Failed to register hotplug callback";
            return false;
        }
    }

    m_timer->start(100);
    return true;
}

void USBDeviceDetector::registerDevice(const USBDeviceInfo &deviceInfo)
{
    QTimer::singleShot(0, this, [=]() {
        if(m_devices.contains(deviceInfo)) {
            return;
        }

        m_devices.append(fillDeviceInfo(deviceInfo));
        emit devicePluggedIn(m_devices.last());
    });
}

void USBDeviceDetector::unregisterDevice(const USBDeviceInfo &deviceInfo)
{
    if(!m_devices.contains(deviceInfo)) {
        return;
    }

    m_devices.removeOne(deviceInfo);
    emit deviceUnplugged(deviceInfo);
}

void USBDeviceDetector::processEvents()
{
    struct timeval timeout = {0, 10000};
    libusb_handle_events_timeout_completed(nullptr, &timeout, nullptr);
}

USBDeviceInfo USBDeviceDetector::fillDeviceInfo(const USBDeviceInfo &deviceInfo)
{
    auto numRetries = 20;
    auto *dev = (libusb_device*)deviceInfo.backendData().value<void*>();

    USBDeviceInfo newinfo = deviceInfo;
    unsigned char buf[1024];

    libusb_device_descriptor desc;
    struct libusb_device_handle *handle;

    bool descOk = false, devOk = false, mfgOk = false, productOk = false, serialOk = false;

    do {
        do {
            descOk = !libusb_get_device_descriptor(dev, &desc);
            if(!descOk) break;

            devOk = !libusb_open(dev, &handle);
            if(!devOk) break;

            mfgOk = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, sizeof(buf)) >= 0;
            if(!mfgOk) break;

            newinfo.setManufacturer(QString::fromLocal8Bit((const char*)buf));

            productOk = libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf)) >= 0;
            if(!productOk) break;

            newinfo.setProductDescription(QString::fromLocal8Bit((const char*)buf));

            serialOk = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, sizeof(buf)) >= 0;
            if(!serialOk) break;

            newinfo.setSerialNumber(QString::fromLocal8Bit((const char*)buf));
        } while(false);

        if(devOk) {
            libusb_close(handle);
        }

        if(serialOk) {
            break;
        }

        QThread::msleep(20);

    } while(--numRetries);

    if(!serialOk) {
        if(!descOk) {
            qCDebug(CATEGORY_DEBUG) << "Failed to get device descriptor";
        } else if(!devOk) {
            qCDebug(CATEGORY_DEBUG) << "Failed to open device";
        } else if(!mfgOk) {
            qCDebug(CATEGORY_DEBUG) << "Failed to get manufacturer string descriptor";
        } else if(!productOk) {
            qCDebug(CATEGORY_DEBUG) << "Failed to get product string descriptor";
        } else {
            qCDebug(CATEGORY_DEBUG) << "Failed to get device serial number";
        }

        return USBDeviceInfo();
    }

    return newinfo;
}

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    Q_UNUSED(ctx)

    auto numRetries = 20;
    auto *detector = (USBDeviceDetector*)user_data;

    libusb_device_descriptor desc;
    bool descOk, valuesOk;

    do {
        descOk = !libusb_get_device_descriptor(dev, &desc);
        valuesOk = desc.idVendor && desc.idProduct;

        if(descOk && valuesOk) {
            break;
        }

        QThread::msleep(20);

    } while(--numRetries);

    if(!valuesOk) {
        qCDebug(CATEGORY_DEBUG) << (descOk ? "Failed to get device descriptor" : "Device descriptor received, but not a valid one");
        return 0;
    }

    const auto info = USBDeviceInfo(desc.idVendor, desc.idProduct).withBackendData(QVariant::fromValue((void*)dev));

    if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        detector->registerDevice(info);
    } else if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        detector->unregisterDevice(info);
    } else {
        qCDebug(CATEGORY_DEBUG) << "Unhandled libusb event";
    }

    return 0;
}
