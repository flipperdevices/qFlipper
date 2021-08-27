#include "deviceinfofetcher.h"

#include <QTimer>
#include <QSerialPort>

#include "device/stm32wb55.h"
#include "serialfinder.h"
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

VCPDeviceInfoFetcher::VCPDeviceInfoFetcher(QObject *parent):
    AbstractDeviceInfoFetcher(parent)
{}

void VCPDeviceInfoFetcher::fetch(const USBDeviceInfo &info)
{
    auto *finder = new SerialFinder(info.serialNumber(), this);
    connect(finder, &SerialFinder::finished, this, &VCPDeviceInfoFetcher::onSerialPortFound);
    connect(finder, &SerialFinder::finished, finder, &QObject::deleteLater);
}

const DeviceInfo &VCPDeviceInfoFetcher::result() const
{
    return m_deviceInfo;
}

void VCPDeviceInfoFetcher::onSerialPortFound(const QSerialPortInfo &portInfo)
{
    if(portInfo.isNull()) {
        setError(QStringLiteral("Failed to find a suitable serial port."));
        return;
    }

    auto *port = new QSerialPort(portInfo, this);

    if(!port->open(QIODevice::ReadWrite)) {
        setError(port->errorString());
        return;
    }

    auto *timeout = new QTimer(this);

    connect(timeout, &QTimer::timeout, this, &AbstractDeviceInfoFetcher::finished);
    connect(this, &AbstractDeviceInfoFetcher::finished, port, &QObject::deleteLater);

    connect(port, &QSerialPort::errorOccurred, this, [=]() {
        timeout->stop();
        setError(port->errorString());
    });

    connect(port, &QSerialPort::readyRead, this, [=]() {
        timeout->start(50);

        while(port->canReadLine()) {
            parseLine(port->readLine());
        }
    });

    const auto success = port->setDataTerminalReady(true) &&
                        (port->write("\rdevice_info\r") > 0) &&
                         port->flush();
    if(success) {
        timeout->start(100);
    }
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

DFUDeviceInfoFetcher::DFUDeviceInfoFetcher(QObject *parent):
    AbstractDeviceInfoFetcher(parent)
{}

void DFUDeviceInfoFetcher::fetch(const USBDeviceInfo &info)
{
    QTimer::singleShot(0, this, [=]() {
        STM32WB55 device(info);

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

