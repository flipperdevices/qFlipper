#pragma once

#include <QObject>
#include <QSerialPortInfo>

#include "deviceinfo.h"
#include "usbdeviceinfo.h"

namespace Flipper {
namespace Zero {

class AbstractDeviceInfoFetcher : public QObject
{
    Q_OBJECT

public:
    AbstractDeviceInfoFetcher(QObject *parent = nullptr);
    virtual ~AbstractDeviceInfoFetcher();

    bool isError() const;
    const QString &errorString() const;

    virtual void fetch(const USBDeviceInfo &info) = 0;
    virtual const DeviceInfo &result() const = 0;

signals:
    void finished();

protected:
    void setError(const QString &errorString);

private:
    bool m_isError;
    QString m_errorString;
};

class VCPDeviceInfoFetcher : public AbstractDeviceInfoFetcher
{
    Q_OBJECT

public:
    VCPDeviceInfoFetcher(QObject *parent = nullptr);

    void fetch(const USBDeviceInfo &info) override;
    const DeviceInfo &result() const override;

private slots:
    void onSerialPortFound(const QSerialPortInfo &portInfo);

private:
    void parseLine(const QByteArray &line);

    DeviceInfo m_deviceInfo;
};

class DFUDeviceInfoFetcher : public AbstractDeviceInfoFetcher
{
    Q_OBJECT

public:
    DFUDeviceInfoFetcher(QObject *parent = nullptr);

    void fetch(const USBDeviceInfo &info) override;
    const DeviceInfo &result() const override;

private:
    DeviceInfo m_deviceInfo;
};

}
}

