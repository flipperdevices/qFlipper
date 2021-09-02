#pragma once

#include <QObject>

#include "usbdeviceinfo.h"

namespace Flipper {
namespace Zero {

class RecoveryController : public QObject
{
    Q_OBJECT

public:
    RecoveryController(USBDeviceInfo info, QObject *parent = nullptr);
    ~RecoveryController();

    const USBDeviceInfo &usbInfo() const;
    void setUSBInfo(const USBDeviceInfo &info);

    bool isError() const;

signals:
    void messageChanged();
    void errorOccured();

private:
    void setMessage(const QString &msg);
    void setError(const QString &msg);

    USBDeviceInfo m_usbInfo;

    bool m_isError;
    QString m_message;
};

}
}
