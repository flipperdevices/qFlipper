#pragma once

#include <QObject>
#include <QSerialPortInfo>

#include "deviceinfo.h"
#include "usbdeviceinfo.h"

class QSerialPort;

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

    virtual void fetch() = 0;
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
    VCPDeviceInfoFetcher(QSerialPort *serialPort, QObject *parent = nullptr);

    void fetch() override;
    const DeviceInfo &result() const override;

private:
    void parseLine(const QByteArray &line);

    QSerialPort *m_serialPort;
    DeviceInfo m_deviceInfo;
};

class DFUDeviceInfoFetcher : public AbstractDeviceInfoFetcher
{
    Q_OBJECT

public:
    DFUDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent = nullptr);

    void fetch() override;
    const DeviceInfo &result() const override;

private:
    USBDeviceInfo m_usbInfo;
    DeviceInfo m_deviceInfo;
};

}
}

