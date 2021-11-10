#include "deviceinfofetcher.h"

#include <QTimer>
#include <QSerialPort>

#include "cli/deviceinfooperation.h"
#include "cli/skipmotdoperation.h"

#include "device/stm32wb55.h"

#include "serialfinder.h"
#include "factoryinfo.h"
#include "macros.h"

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
    m_serialPort(nullptr)
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

    auto *skipMotd = new SkipMOTDOperation(m_serialPort, this);
    connect(skipMotd, &AbstractOperation::finished, this, &VCPDeviceInfoFetcher::onMOTDSkipped);
    skipMotd->start();
}

void VCPDeviceInfoFetcher::onMOTDSkipped()
{
    auto *skipMotd = qobject_cast<SkipMOTDOperation*>(sender());

    if(skipMotd->isError()) {
        finishWithError(skipMotd->errorString());
    } else {
        auto *getInfo = new DeviceInfoOperation(m_serialPort, this);
        connect(getInfo, &AbstractOperation::finished, this, &VCPDeviceInfoFetcher::onDeviceInfoRead);
        getInfo->start();
    }

    skipMotd->deleteLater();
}

void VCPDeviceInfoFetcher::onDeviceInfoRead()
{
    auto *getInfo = qobject_cast<DeviceInfoOperation*>(sender());

    if(getInfo->isError()) {
        finishWithError(getInfo->errorString());
    } else {
        // TODO: Is there a better way to do this?
        const auto &info = getInfo->result();
        m_deviceInfo.name = info.name;
        m_deviceInfo.bootloader = info.bootloader;
        m_deviceInfo.firmware = info.firmware;
        m_deviceInfo.hardware = info.hardware;
        m_deviceInfo.fusVersion = info.fusVersion;
        m_deviceInfo.radioVersion = info.radioVersion;

        if(m_deviceInfo.name.isEmpty()) {
            finishWithError(QStringLiteral("Failed to read device factory information"));
        } else {
            finish();
        }
    }

    getInfo->deleteLater();
}

void VCPDeviceInfoFetcher::finish()
{
    emit finished();
}

using namespace STM32;

DFUDeviceInfoFetcher::DFUDeviceInfoFetcher(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoFetcher(parent)
{
    m_deviceInfo.usbInfo = info;
    m_deviceInfo.systemLocation = QStringLiteral("S/N:%1").arg(info.serialNumber());
    m_deviceInfo.storage.isExternalPresent = false;
    m_deviceInfo.storage.isAssetsPresent = false;
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

