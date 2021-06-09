#include "libusbdevicedetector.h"
#include "libusbbackend.h"

#include <QTimer>
#include <QTimerEvent>

#include <libusb.h>

#include "macros.h"

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);

/* Temporary (hopefully) hacks for Windows begin */

class DeviceWatcher {
    struct List {
        libusb_device **head = nullptr;
        ssize_t size = 0;
    };

public:
    DeviceWatcher(const QList <USBDeviceParams> &wanted);
    QList <libusb_device*> devicesArrived(const List &curList);
    QList <libusb_device*> devicesLeft(const List &curList);

    void update();

private:
    QList <USBDeviceParams> m_wanted;
    USBBackend *m_backend;
    List m_prevList;
};

DeviceWatcher::DeviceWatcher(const QList <USBDeviceParams> &wanted):
    m_wanted(wanted)
{}

QList <libusb_device*> DeviceWatcher::devicesArrived(const List &curList)
{
    // What is in curList that is not in the m_prevList?
    QList <libusb_device*> ret;

    for(auto i = 0; i < curList.size; ++i) {
        auto *devToCheck = curList.head[i];
        auto found = false;

        for(auto j = 0; j < m_prevList.size; ++j) {
            if((found = (m_prevList.head[j] == devToCheck))) break;
        }

        if(!found) {
            ret.append(devToCheck);
        }
    }

    return ret;
}

QList <libusb_device*> DeviceWatcher::devicesLeft(const List &curList)
{
    // What is in m_prevList that is not in the m_curList?
    QList <libusb_device*> ret;

    for(auto i = 0; i < m_prevList.size; ++i) {
        auto *devToCheck = m_prevList.head[i];
        auto found = false;

        for(auto j = 0; j < curList.size; ++j) {
            if((found = (curList.head[j] == devToCheck))) break;
        }

        if(!found) {
            ret.append(devToCheck);
        }
    }

    return ret;
}

void DeviceWatcher::update()
{
    List curList;

    curList.size = libusb_get_device_list(nullptr, &curList.head);

    const auto arrived = devicesArrived(curList);
    const auto left = devicesLeft(curList);

    for(auto *dev : arrived) {
        // if it is in the hotplug wanted list
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(dev, &desc);

        auto it = std::find_if(m_wanted.cbegin(), m_wanted.cend(), [&desc](const USBDeviceParams &p) {
            return p.vendorID == (desc.idVendor) && (p.productID == desc.idProduct);
        });

        if(it != m_wanted.cend()) {
            const USBDeviceParams params = {
                desc.idVendor,
                desc.idProduct,
                "", "", "",
                dev
            };

            QTimer::singleShot(0, m_backend, [=]() {
                const auto newParameters = m_backend->getExtraDeviceInfo(params);
                // TODO: check for manufacturer and product
                emit USBBackend::instance()->detector()->devicePluggedIn(newParameters);
            });
        }
    }

    for(auto *dev : left) {
        // if it is in the hotplug wanted list
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(dev, &desc);

        auto it = std::find_if(m_wanted.cbegin(), m_wanted.cend(), [&desc](const USBDeviceParams &p) {
            return p.vendorID == (desc.idVendor) && (p.productID == desc.idProduct);
        });

        if(it != m_wanted.cend()) {
            const USBDeviceParams params = {
                desc.idVendor,
                desc.idProduct,
                "", "", "",
                dev
            };

            emit USBBackend::instance()->detector()->deviceUnplugged(params);
        }
    }

    if(m_prevList.head) {
        libusb_free_device_list(m_prevList.head, 0);
    }

    m_prevList = curList;
}

/* Windows hacks end */

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent)
{}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceParams> &wantedList)
{
    if(!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        info_msg("No hotplug support, falling back to dumb polling");
        m_watcher = new DeviceWatcher(wantedList);

    } else {
        for(const auto &params : wantedList) {
            const auto events = libusb_hotplug_event(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT);
            const auto err = libusb_hotplug_register_callback(nullptr, events, LIBUSB_HOTPLUG_ENUMERATE, params.vendorID,
                                                              params.productID, LIBUSB_HOTPLUG_MATCH_ANY, libusbHotplugCallback, this, nullptr);

            check_return_bool(!err, "Failed to register hotplug callback");
        }
    }

    return startTimer(100);
}

void USBDeviceDetector::timerEvent(QTimerEvent *e)
{
    e->accept();

    if(libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        struct timeval timeout = {0, 10000};
        libusb_handle_events_timeout_completed(nullptr, &timeout, nullptr);
    } else {
        m_watcher->update();
    }
}

static int libusbHotplugCallback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    Q_UNUSED(ctx)

    auto *detector = static_cast<USBDeviceDetector*>(user_data);

    libusb_device_descriptor desc;
    check_return_val(!libusb_get_device_descriptor(dev, &desc),"Failed to get device descriptor", 0);

    const USBDeviceParams params = {
        desc.idVendor,
        desc.idProduct,
        "", "", "",
        dev
    };

    if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        // Get string descriptors out of callback context
        QTimer::singleShot(0, detector, [=]() {
            const auto newParameters = USBBackend::instance()->getExtraDeviceInfo(params);
            // TODO: check for manufacturer and product
            emit detector->devicePluggedIn(newParameters);
        });

    } else if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        emit detector->deviceUnplugged(params);
    } else {
        info_msg("Unhandled libusb event");
    }

    return 0;
}
