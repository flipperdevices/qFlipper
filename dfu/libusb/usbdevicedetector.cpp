#include "usbdevicedetector.h"

#include <libusb.h>

#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_DETECTOR, "USB")

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000107)
static void libusbLogCallback(libusb_context *ctx, libusb_log_level logLevel, const char *text);
#endif

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent),
    m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &USBDeviceDetector::processEvents);
    libusb_init(nullptr);
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000107)
    libusb_set_log_cb(nullptr, libusbLogCallback, LIBUSB_LOG_CB_GLOBAL);
#endif
}

USBDeviceDetector::~USBDeviceDetector()
{
    libusb_exit(nullptr);
}

void USBDeviceDetector::setLogLevel(int logLevel)
{
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000107)
    if(logLevel> LIBUSB_LOG_LEVEL_DEBUG) {
        qCDebug(LOG_DETECTOR) << "Invalid log level:" << logLevel;
        return;
    }

    libusb_set_option(nullptr, LIBUSB_OPTION_LOG_LEVEL, logLevel);
#else
    Q_UNUSED(logLevel)
    qCDebug(LOG_DETECTOR) << "Setting log level is supported with libusb >= 1.0.23";
#endif
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
    if(!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        qCDebug(LOG_DETECTOR) << "Failed to get device descriptor";
        return false;
    }

    m_timer->stop();

    for(const auto &info : wantedList) {
        const auto events = libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);
        const auto err = libusb_hotplug_register_callback(nullptr, events, LIBUSB_HOTPLUG_ENUMERATE, info.vendorID(),
                                                          info.productID(), LIBUSB_HOTPLUG_MATCH_ANY, libusbHotplugCallback, this, nullptr);
        if(err) {
            qCDebug(LOG_DETECTOR) << "Failed to register hotplug callback";
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
            qCDebug(LOG_DETECTOR) << "Failed to get device descriptor";
        } else if(!devOk) {
            qCDebug(LOG_DETECTOR) << "Failed to open device";
        } else if(!mfgOk) {
            qCDebug(LOG_DETECTOR) << "Failed to get manufacturer string descriptor";
        } else if(!productOk) {
            qCDebug(LOG_DETECTOR) << "Failed to get product string descriptor";
        } else {
            qCDebug(LOG_DETECTOR) << "Failed to get device serial number";
        }
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
        qCDebug(LOG_DETECTOR) << (descOk ? "Failed to get device descriptor" : "Device descriptor received, but not a valid one");
        return 0;
    }

    const auto info = USBDeviceInfo(desc.idVendor, desc.idProduct).withBackendData(QVariant::fromValue((void*)dev));

    if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        detector->registerDevice(info);
    } else if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        detector->unregisterDevice(info);
    } else {
        qCDebug(LOG_DETECTOR) << "Unhandled libusb event";
    }

    return 0;
}

#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x01000107)
static void libusbLogCallback(libusb_context *ctx, libusb_log_level logLevel, const char *text) {
    Q_UNUSED(ctx)

    static const QStringList logPrefix = {
        QStringLiteral("NONE"),
        QStringLiteral("ERR"),
        QStringLiteral("WRN"),
        QStringLiteral("INF"),
        QStringLiteral("DBG"),
    };

    if(logLevel >= logPrefix.size()) {
        qCDebug(LOG_DETECTOR) << "Invalid log level:" << logLevel;
        return;
    }

    qCDebug(LOG_DETECTOR).noquote() << QStringLiteral("[%1]").arg(logPrefix[logLevel]) << QString::fromLatin1(text, strlen(text) - 1);
}
#endif
