#pragma once

#include <QObject>
#include <QByteArray>

#include "abstractoperationhelper.h"
#include "flipperzero/deviceinfo.h"

class QTimer;
class QSerialPort;

namespace Flipper {
namespace Zero {

class AbstractDeviceInfoHelper : public AbstractOperationHelper
{
    Q_OBJECT

public:
    AbstractDeviceInfoHelper(QObject *parent = nullptr);
    virtual ~AbstractDeviceInfoHelper();

    static AbstractDeviceInfoHelper *create(const USBDeviceInfo &info, QObject *parent = nullptr);
    const DeviceInfo &result() const;

protected:
    DeviceInfo m_deviceInfo;
};

class VCPDeviceInfoHelper : public AbstractDeviceInfoHelper
{
    Q_OBJECT

    enum OperationState {
        FindingSerialPort = AbstractOperationHelper::User,
        SkippingMOTD,
        StartingRPCSession,
        FetchingDeviceInfo,
        CheckingSDCard,
        CheckingManifest
    };

public:
    VCPDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent = nullptr);

private:
    void nextStateLogic() override;

    void findSerialPort();
    void skipMOTD();
    void startRPCSession();
    void fetchDeviceInfo();
    void checkSDCard();
    void checkManifest();

private:
    QSerialPort *serialPort() const;
};

class DFUDeviceInfoHelper : public AbstractDeviceInfoHelper
{
    Q_OBJECT

public:
    DFUDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent = nullptr);

private:
    void nextStateLogic() override;
};

}
}
