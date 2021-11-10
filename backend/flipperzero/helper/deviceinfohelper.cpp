#include "deviceinfohelper.h"

#include <cmath>

#include <QTimer>
#include <QSerialPort>

#include "flipperzero/cli/deviceinfooperation.h"
#include "flipperzero/cli/skipmotdoperation.h"
#include "flipperzero/cli/statoperation.h"
#include "flipperzero/factoryinfo.h"

#include "device/stm32wb55.h"

#include "serialfinder.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

AbstractDeviceInfoHelper::AbstractDeviceInfoHelper(QObject *parent):
    AbstractOperationHelper(parent)
{}

AbstractDeviceInfoHelper::~AbstractDeviceInfoHelper()
{}

AbstractDeviceInfoHelper *AbstractDeviceInfoHelper::create(const USBDeviceInfo &info, QObject *parent)
{
    const auto pid = info.productID();

    if(pid == 0x5740) {
        return new VCPDeviceInfoHelper(info, parent);
    } else if(pid == 0xdf11) {
        return new DFUDeviceInfoHelper(info, parent);
    } else {
        error_msg("Not a Flipper Zero device.")
    }

    return nullptr;
}

VCPDeviceInfoHelper::VCPDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoHelper(parent),
    m_serialPort(nullptr)
{
    m_deviceInfo.usbInfo = info;
}

const DeviceInfo &VCPDeviceInfoHelper::result() const
{
    return m_deviceInfo;
}

void VCPDeviceInfoHelper::nextStateLogic()
{
    if(state() == AbstractDeviceInfoHelper::Ready) {
        setState(VCPDeviceInfoHelper::FindingSerialPort);
        findSerialPort();

    } else if(state() == VCPDeviceInfoHelper::FindingSerialPort) {
        setState(VCPDeviceInfoHelper::SkippingMOTD);
        skipMOTD();

    } else if(state() == VCPDeviceInfoHelper::SkippingMOTD) {
        setState(VCPDeviceInfoHelper::FetchingDeviceInfo);
        fetchDeviceInfo();

    } else if(state() == VCPDeviceInfoHelper::FetchingDeviceInfo) {
        setState(VCPDeviceInfoHelper::CheckingSDCard);
        checkSDCard();

    } else if(state() == VCPDeviceInfoHelper::CheckingSDCard) {
        setState(VCPDeviceInfoHelper::CheckingManifest);
        checkManifest();

    } else if(state() == VCPDeviceInfoHelper::CheckingManifest) {
        finish();
    }
}

void VCPDeviceInfoHelper::findSerialPort()
{
    auto *finder = new SerialFinder(m_deviceInfo.usbInfo.serialNumber(), this);

    connect(finder, &SerialFinder::finished, this, [=](const QSerialPortInfo &portInfo) {
        if(portInfo.isNull()) {
            finishWithError(QStringLiteral("Invalid serial port info."));

        } else {
            m_deviceInfo.serialInfo = portInfo;
            m_deviceInfo.systemLocation = portInfo.systemLocation();

            m_serialPort = new QSerialPort(portInfo, this);
            if(!m_serialPort->open(QIODevice::ReadWrite)) {
                finishWithError(m_serialPort->errorString());
            } else {
                advanceState();
            }
        }
    });
}

void VCPDeviceInfoHelper::skipMOTD()
{
    auto *operation = new SkipMOTDOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        } else {
            advanceState();
        }
    });

    operation->start();
}

void VCPDeviceInfoHelper::fetchDeviceInfo()
{
    auto *operation = new DeviceInfoOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());

        } else {
            // TODO: Is there a better way to do this?
            const auto &info = operation->result();
            m_deviceInfo.name = info.name;
            m_deviceInfo.bootloader = info.bootloader;
            m_deviceInfo.firmware = info.firmware;
            m_deviceInfo.hardware = info.hardware;
            m_deviceInfo.fusVersion = info.fusVersion;
            m_deviceInfo.radioVersion = info.radioVersion;

            if(m_deviceInfo.name.isEmpty()) {
                finishWithError(QStringLiteral("Failed to read device factory information"));
            } else {
                advanceState();
            }
        }
    });

    operation->start();
}

void VCPDeviceInfoHelper::checkSDCard()
{
    auto *operation = new StatOperation(m_serialPort, QByteArrayLiteral("/ext"), this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());

        } else if(operation->type() != StatOperation::Type::Storage) {
            m_deviceInfo.storage.isExternalPresent = false;
            m_deviceInfo.storage.isAssetsInstalled = false;
            finish();

        } else {
            m_deviceInfo.storage.isExternalPresent = true;
            m_deviceInfo.storage.externalFree = floor((double)operation->sizeFree() * 100.0 / (double)operation->size());
            advanceState();
        }
    });

    operation->start();
}

void VCPDeviceInfoHelper::checkManifest()
{
    auto *operation = new StatOperation(m_serialPort, QByteArrayLiteral("/ext/Manifest"), this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());

        } else {
            m_deviceInfo.storage.isAssetsInstalled = (operation->type() == StatOperation::Type::RegularFile);
            advanceState();
        }
    });

    operation->start();
}

using namespace STM32;

DFUDeviceInfoHelper::DFUDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoHelper(parent)
{
    m_deviceInfo.usbInfo = info;
    m_deviceInfo.systemLocation = QStringLiteral("S/N:%1").arg(info.serialNumber());
    m_deviceInfo.storage.isExternalPresent = false;
    m_deviceInfo.storage.isAssetsInstalled = false;
}

const DeviceInfo &DFUDeviceInfoHelper::result() const
{
    return m_deviceInfo;
}

void DFUDeviceInfoHelper::nextStateLogic()
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
