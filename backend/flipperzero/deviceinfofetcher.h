#pragma once

#include <QObject>
#include <QByteArray>

#include "failable.h"
#include "deviceinfo.h"
#include "usbdeviceinfo.h"

class QTimer;
class QSerialPort;

namespace Flipper {
namespace Zero {

class AbstractDeviceInfoFetcher : public QObject, public Failable
{
    Q_OBJECT

public:
    AbstractDeviceInfoFetcher(QObject *parent = nullptr);
    virtual ~AbstractDeviceInfoFetcher();

    static AbstractDeviceInfoFetcher *create(const USBDeviceInfo &info, QObject *parent = nullptr);
    virtual const DeviceInfo &result() const = 0;

signals:
    void finished();

protected slots:
    virtual void fetch() = 0;

protected:
    virtual void finish() = 0;
    void finishWithError(const QString &errorString);
};

class VCPDeviceInfoFetcher : public AbstractDeviceInfoFetcher
{
    Q_OBJECT

public:
    VCPDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent = nullptr);
    const DeviceInfo &result() const override;

private slots:
    void fetch() override;
    void onSerialPortFound(const QSerialPortInfo &portInfo);
    void onMOTDSkipped();
    void onDeviceInfoRead();

private:
    void finish() override;

    QSerialPort *m_serialPort;
    DeviceInfo m_deviceInfo;
};

class DFUDeviceInfoFetcher : public AbstractDeviceInfoFetcher
{
    Q_OBJECT

public:
    DFUDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent = nullptr);
    const DeviceInfo &result() const override;

private slots:
    void fetch() override;

private:
    void finish() override;

    DeviceInfo m_deviceInfo;
};

}
}

