#include "deviceinfofetcher.h"

#include <QTimer>
#include <QSerialPort>

#include "device/stm32wb55.h"
#include "factoryinfo.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

AbstractDeviceInfoFetcher::AbstractDeviceInfoFetcher(QObject *parent):
    QObject(parent),
    m_isError(false),
    m_errorString(QStringLiteral("No error"))
{}

AbstractDeviceInfoFetcher::~AbstractDeviceInfoFetcher()
{}

bool AbstractDeviceInfoFetcher::isError() const
{
    return m_isError;
}

const QString &AbstractDeviceInfoFetcher::errorString() const
{
    return m_errorString;
}

void AbstractDeviceInfoFetcher::setError(const QString &errorString)
{
    m_isError = true;
    m_errorString = errorString;

    emit finished();
}

VCPDeviceInfoFetcher::VCPDeviceInfoFetcher(QSerialPort *serialPort, QObject *parent):
    AbstractDeviceInfoFetcher(parent),
    m_serialPort(serialPort)
{}

void VCPDeviceInfoFetcher::fetch()
{
    if(!m_serialPort->open(QIODevice::ReadWrite)) {
        setError(m_serialPort->errorString());
        return;
    }

    auto *timeout = new QTimer(this);

    connect(timeout, &QTimer::timeout, this, &AbstractDeviceInfoFetcher::finished);
    connect(this, &AbstractDeviceInfoFetcher::finished, m_serialPort, &QSerialPort::close);

    connect(m_serialPort, &QSerialPort::errorOccurred, this, [=]() {
        timeout->stop();
        setError(m_serialPort->errorString());
    });

    connect(m_serialPort, &QSerialPort::readyRead, this, [=]() {
        timeout->start(50);

        while(m_serialPort->canReadLine()) {
            parseLine(m_serialPort->readLine());
        }
    });

    const auto success = m_serialPort->setDataTerminalReady(true) &&
                        (m_serialPort->write("\rdevice_info\r") > 0) &&
                         m_serialPort->flush();
    if(success) {
        timeout->start(100);
    }
}

const DeviceInfo &VCPDeviceInfoFetcher::result() const
{
    return m_deviceInfo;
}

void VCPDeviceInfoFetcher::parseLine(const QByteArray &line)
{
    if(line.count(':') != 1) {
        return;
    }

    const auto validx = line.indexOf(':');
    const auto value = line.mid(validx + 1).trimmed();

    if(line.startsWith(QByteArrayLiteral("hardware_name"))) {
        m_deviceInfo.name = value;
    } else if(line.startsWith(QByteArrayLiteral("hardware_target"))) {
        m_deviceInfo.target = QStringLiteral("f") + value;
    } else if(line.startsWith(QByteArrayLiteral("firmware_version"))) {
        m_deviceInfo.firmware.version = value;
    } else {}
}

using namespace STM32;

DFUDeviceInfoFetcher::DFUDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoFetcher(parent),
    m_usbInfo(info)
{}

void DFUDeviceInfoFetcher::fetch()
{
    QTimer::singleShot(0, this, [=]() {
        STM32WB55 device(m_usbInfo);

        if(!device.beginTransaction()) {
            setError(QStringLiteral("Failed to initiate transaction"));
            return;
        }

        const FactoryInfo factoryInfo(device.OTPData(FactoryInfo::size()));

        if(!device.endTransaction()) {
            setError(QStringLiteral("Failed to end transaction"));
            return;
        }

        if(!factoryInfo.isValid()) {
            setError(QStringLiteral("Failed to read device factory information"));
            return;
        }

        m_deviceInfo.name = factoryInfo.name();
        m_deviceInfo.target = QStringLiteral("f%1").arg(factoryInfo.target());
        m_deviceInfo.color = (DeviceInfo::Color)factoryInfo.color();

        emit finished();
    });
}

const DeviceInfo &DFUDeviceInfoFetcher::result() const
{
    return m_deviceInfo;
}

