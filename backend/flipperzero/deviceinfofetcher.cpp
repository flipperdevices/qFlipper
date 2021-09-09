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

AbstractDeviceInfoFetcher *AbstractDeviceInfoFetcher::create(const USBDeviceInfo &info, QObject *parent)
{
    const auto pid = info.productID();

    if(pid == 0x5740) {
        return new VCPDeviceInfoFetcher(info, parent);
    } else if(pid == 0xdf11) {
        return new DFUDeviceInfoFetcher(info, parent);
    } else {
        error_msg("Not a Flipper Zero device.")
    }

    return nullptr;
}

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

VCPDeviceInfoFetcher::VCPDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoFetcher(parent)
{
    m_deviceInfo.usbInfo = info;
}

void VCPDeviceInfoFetcher::fetch()
{
    auto *finder = new SerialFinder(m_deviceInfo.usbInfo.serialNumber(), this);
    connect(finder, &SerialFinder::finished, this, &VCPDeviceInfoFetcher::onSerialPortFound);
}

const DeviceInfo &VCPDeviceInfoFetcher::result() const
{
    return m_deviceInfo;
}

void VCPDeviceInfoFetcher::onSerialPortFound(const QSerialPortInfo &portInfo)
{
    if(portInfo.isNull()) {
        setError(QStringLiteral("Invalid serial port info."));
        return;
    }

    m_deviceInfo.serialInfo = portInfo;

    auto *serialPort = new QSerialPort(portInfo, this);

    if(!serialPort->open(QIODevice::ReadWrite)) {
        setError(serialPort->errorString());
        return;
    }

    auto *timeout = new QTimer(this);

    connect(timeout, &QTimer::timeout, this, &AbstractDeviceInfoFetcher::finished);
    connect(this, &AbstractDeviceInfoFetcher::finished, serialPort, &QSerialPort::close);

    connect(serialPort, &QSerialPort::errorOccurred, this, [=]() {
        timeout->stop();
        setError(serialPort->errorString());
    });

    connect(serialPort, &QSerialPort::readyRead, this, [=]() {
        timeout->start(50);

        while(serialPort->canReadLine()) {
            parseLine(serialPort->readLine());
        }
    });

    const auto success = serialPort->setDataTerminalReady(true) &&
                        (serialPort->write("\rdevice_info\r\n") > 0);
    if(success) {
        timeout->start(5000);
    }
}

void VCPDeviceInfoFetcher::parseLine(const QByteArray &line)
{
    // TODO: Add more fields
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
    AbstractDeviceInfoFetcher(parent)
{
    m_deviceInfo.usbInfo = info;
}

void DFUDeviceInfoFetcher::fetch()
{
    QTimer::singleShot(0, this, [=]() {
        STM32WB55 device(m_deviceInfo.usbInfo);

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

