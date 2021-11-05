#include "deviceinfofetcher.h"

#include <QTimer>
#include <QBuffer>
#include <QSerialPort>

#include "cli/skipmotdoperation.h"
#include "device/stm32wb55.h"
#include "serialfinder.h"
#include "factoryinfo.h"
#include "macros.h"

#define RESPONSE_TIMEOUT_MS 5000
#define PROMPT_READY "\r\n>: \a"

using namespace Flipper;
using namespace Zero;

AbstractDeviceInfoFetcher::AbstractDeviceInfoFetcher(QObject *parent):
    QObject(parent)
{
    QTimer::singleShot(0, this, &AbstractDeviceInfoFetcher::fetch);
}

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

void AbstractDeviceInfoFetcher::finishWithError(const QString &errorString)
{
    setError(errorString);
    finish();
}

VCPDeviceInfoFetcher::VCPDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoFetcher(parent),
    m_responseTimer(new QTimer(this)),
    m_serialPort(nullptr)
{
    m_deviceInfo.usbInfo = info;
    m_responseTimer->setSingleShot(true);
    connect(m_responseTimer, &QTimer::timeout, this, &VCPDeviceInfoFetcher::onResponseTimeout);
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
        finishWithError(QStringLiteral("Invalid serial port info."));
        return;
    }

    m_deviceInfo.serialInfo = portInfo;
    m_deviceInfo.systemLocation = portInfo.systemLocation();

    m_serialPort = new QSerialPort(portInfo, this);

    if(!m_serialPort->open(QIODevice::ReadWrite)) {
        finishWithError(m_serialPort->errorString());
        return;
    }

    auto *skipper = new SkipMOTDOperation(m_serialPort, this);

    connect(skipper, &AbstractOperation::finished, this, [=]() {

        if(skipper->isError()) {
            finishWithError(skipper->errorString());
        } else if(m_serialPort->write("\rdevice_info\r\n") < 0) {
            finishWithError(m_serialPort->errorString());
        } else {
            connect(m_serialPort, &QSerialPort::readyRead, this, &VCPDeviceInfoFetcher::onSerialPortReadyRead);
            connect(m_serialPort, &QSerialPort::errorOccurred, this, &VCPDeviceInfoFetcher::onSerialPortErrorOccured);

            m_responseTimer->start(RESPONSE_TIMEOUT_MS);
        }

        skipper->deleteLater();
    });

    skipper->start();
}

void VCPDeviceInfoFetcher::onSerialPortReadyRead()
{
    m_responseTimer->start(RESPONSE_TIMEOUT_MS);
    m_receivedData += m_serialPort->readAll();

    if(m_receivedData.endsWith(PROMPT_READY)) {
        parseReceivedData();

        if(m_deviceInfo.name.isEmpty()) {
            finishWithError(QStringLiteral("Failed to read device factory information"));
        } else {
            finish();
        }
    }
}

void VCPDeviceInfoFetcher::onSerialPortErrorOccured()
{
    finishWithError(m_serialPort->errorString());
}

void VCPDeviceInfoFetcher::onResponseTimeout()
{
    finishWithError(QStringLiteral("Operation timeout"));
}

void VCPDeviceInfoFetcher::finish()
{
    m_responseTimer->stop();
    emit finished();
}

void VCPDeviceInfoFetcher::parseReceivedData()
{
    QBuffer buf(&m_receivedData, this);

    if(!buf.open(QIODevice::ReadOnly)) {
        finishWithError(buf.errorString());
        return;
    }

    m_deviceInfo.fusVersion = QStringLiteral("0.0.0");
    m_deviceInfo.radioVersion = QStringLiteral("0.0.0");

    while(buf.canReadLine()) {
        parseLine(buf.readLine());
    }

    buf.close();
}

void VCPDeviceInfoFetcher::parseLine(const QByteArray &line)
{
    // TODO: Add more fields
    if(line.count(':') != 1) {
        return;
    }

    const auto validx = line.indexOf(':');
    const auto key = line.left(validx).trimmed();
    const auto value = line.mid(validx + 1).trimmed();

    if(key == QByteArrayLiteral("hardware_name")) {
        m_deviceInfo.name = value;
    } else if(key == QByteArrayLiteral("hardware_target")) {
        m_deviceInfo.hardware.target = QStringLiteral("f") + value;
    } else if(key == QByteArrayLiteral("hardware_ver")) {
        m_deviceInfo.hardware.version = value;
    } else if(key == QByteArrayLiteral("hardware_body")) {
        m_deviceInfo.hardware.body = QStringLiteral("b") + value;
    } else if(key == QByteArrayLiteral("hardware_connect")) {
        m_deviceInfo.hardware.connect = QStringLiteral("c") + value;
    } else if(key == QByteArrayLiteral("hardware_color")) {
        m_deviceInfo.hardware.color = (HardwareInfo::Color)value.toInt();
    } else if(key == QByteArrayLiteral("firmware_version")) {
        m_deviceInfo.firmware.version = value;
    } else if(key == QByteArrayLiteral("firmware_commit")) {
        m_deviceInfo.firmware.commit = value;
    } else if(key == QByteArrayLiteral("firmware_branch")) {
        m_deviceInfo.firmware.branch = value;
    } else if(key == QByteArrayLiteral("firmware_build_date")) {
        m_deviceInfo.firmware.date = QDateTime::fromString(value, QStringLiteral("dd-MM-yyyy"));

    } else if(key == QByteArrayLiteral("radio_stack_major")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(0, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_stack_minor")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(1, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_stack_sub")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(2, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_major")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(0, value);
        m_deviceInfo.fusVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_minor")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(1, value);
        m_deviceInfo.fusVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_sub")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(2, value);
        m_deviceInfo.fusVersion = fields.join('.');
    } else {}
}

using namespace STM32;

DFUDeviceInfoFetcher::DFUDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoFetcher(parent)
{
    m_deviceInfo.usbInfo = info;
    m_deviceInfo.systemLocation = QStringLiteral("S/N:%1").arg(info.serialNumber());
}

void DFUDeviceInfoFetcher::fetch()
{
    STM32WB55 device(m_deviceInfo.usbInfo);

    if(!device.beginTransaction()) {
        finishWithError(QStringLiteral("Failed to initiate transaction"));
        return;
    }

    const FactoryInfo factoryInfo(device.OTPData(FactoryInfo::size()));

    if(!device.endTransaction()) {
        finishWithError(QStringLiteral("Failed to end transaction"));
        return;
    }

    if(!factoryInfo.isValid()) {
        finishWithError(QStringLiteral("Failed to read device factory information"));
        return;
    }

    m_deviceInfo.name = factoryInfo.name();
    m_deviceInfo.hardware.version = QString::number(factoryInfo.version());
    m_deviceInfo.hardware.target = QStringLiteral("f%1").arg(factoryInfo.target());
    m_deviceInfo.hardware.body = QStringLiteral("b%1").arg(factoryInfo.body());
    m_deviceInfo.hardware.connect = QStringLiteral("c%1").arg(factoryInfo.connect());
    m_deviceInfo.hardware.color = (HardwareInfo::Color)factoryInfo.color();

    finish();
}

const DeviceInfo &DFUDeviceInfoFetcher::result() const
{
    return m_deviceInfo;
}

void DFUDeviceInfoFetcher::finish()
{
    emit finished();
}

