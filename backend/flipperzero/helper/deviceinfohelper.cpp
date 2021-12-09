#include "deviceinfohelper.h"

#include <cmath>

#include <QTimer>
#include <QSerialPort>

#include "flipperzero/cli/storageinfooperation.h"
#include "flipperzero/cli/deviceinfooperation.h"
#include "flipperzero/cli/skipmotdoperation.h"
#include "flipperzero/cli/startrpcoperation.h"
#include "flipperzero/cli/stoprpcoperation.h"
#include "flipperzero/cli/statoperation.h"
#include "flipperzero/factoryinfo.h"

#include "device/stm32wb55.h"

#include "serialfinder.h"
#include "debug.h"

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
        setState(VCPDeviceInfoHelper::StartingRPCSession);
        startRPCSession();

    } else if(state() == VCPDeviceInfoHelper::StartingRPCSession) {
        setState(VCPDeviceInfoHelper::FetchingDeviceInfo);
        fetchDeviceInfo();

    } else if(state() == VCPDeviceInfoHelper::FetchingDeviceInfo) {
//        setState(VCPDeviceInfoHelper::StoppingRPCSession);
//        stopRPCSession();
        setState(VCPDeviceInfoHelper::CheckingSDCard);
        checkSDCard();

    } else if(state() == VCPDeviceInfoHelper::CheckingSDCard) {
//        setState(VCPDeviceInfoHelper::CheckingManifest);
//        checkManifest();
        setState(VCPDeviceInfoHelper::StoppingRPCSession);
        stopRPCSession();

    } else if(state() == VCPDeviceInfoHelper::CheckingManifest) {
        setState(VCPDeviceInfoHelper::StoppingRPCSession);
        stopRPCSession();

    } else if(state() == VCPDeviceInfoHelper::StoppingRPCSession) {
        closePortAndFinish();
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

        operation->deleteLater();
    });

    operation->start();
}

void VCPDeviceInfoHelper::startRPCSession()
{
    auto *operation = new StartRPCOperation(m_serialPort, this);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        } else {
            advanceState();
        }

        operation->deleteLater();
    });

    operation->start();
}

void VCPDeviceInfoHelper::fetchDeviceInfo()
{
    auto *operation = new DeviceInfoOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
            return;
        }

        m_deviceInfo.name = operation->result(QByteArrayLiteral("hardware_name"));

        m_deviceInfo.bootloader = {
            .version = operation->result(QByteArrayLiteral("bootloader_version")),
            .commit = operation->result(QByteArrayLiteral("bootloader_commit")),
            .branch = operation->result(QByteArrayLiteral("bootloader_branch")),
            .date = QDateTime::fromString(operation->result(QByteArrayLiteral("bootloader_build_date")), "MM-dd-yyyy").date()
        };

        m_deviceInfo.firmware = {
            .version = operation->result(QByteArrayLiteral("firmware_version")),
            .commit = operation->result(QByteArrayLiteral("firmware_commit")),
            .branch = operation->result(QByteArrayLiteral("firmware_branch")),
            .date = QDateTime::fromString(operation->result(QByteArrayLiteral("firmware_build_date")), "MM-dd-yyyy").date()
        };

        m_deviceInfo.hardware = {
            .version = operation->result(QByteArrayLiteral("hardware_ver")),
            .target = QByteArrayLiteral("f") + operation->result(QByteArrayLiteral("hardware_target")),
            .body = QByteArrayLiteral("b") + operation->result(QByteArrayLiteral("hardware_body")),
            .connect = QByteArrayLiteral("c") + operation->result(QByteArrayLiteral("hardware_connect")),
            .color = (HardwareInfo::Color)operation->result(QByteArrayLiteral("hardware_color")).toInt(),
        };

        m_deviceInfo.fusVersion = QStringLiteral("%1.%2.%3").arg(
            operation->result(QByteArrayLiteral("radio_fus_major")),
            operation->result(QByteArrayLiteral("radio_fus_minor")),
            operation->result(QByteArrayLiteral("radio_fus_sub")));

        m_deviceInfo.radioVersion = QStringLiteral("%1.%2.%3").arg(
            operation->result(QByteArrayLiteral("radio_stack_major")),
            operation->result(QByteArrayLiteral("radio_stack_minor")),
            operation->result(QByteArrayLiteral("radio_stack_sub")));

        // Temporary
        m_deviceInfo.storage.isExternalPresent = false;
        m_deviceInfo.storage.isAssetsInstalled = false;

        if(m_deviceInfo.name.isEmpty()) {
            finishWithError(QStringLiteral("Failed to read device information"));
        } else {
            advanceState();
        }

        operation->deleteLater();
    });

    operation->start();
}

void VCPDeviceInfoHelper::checkSDCard()
{
    auto *operation = new StorageInfoOperation(m_serialPort, QByteArrayLiteral("/ext"), this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());

        } else if(!operation->isPresent()) {
            m_deviceInfo.storage.isExternalPresent = false;
            m_deviceInfo.storage.isAssetsInstalled = false;
            closePortAndFinish();

        } else {
            m_deviceInfo.storage.isExternalPresent = true;
            m_deviceInfo.storage.externalFree = floor((double)operation->sizeFree() * 100.0 /
                                                      (double)operation->sizeTotal());
            advanceState();
        }

        operation->deleteLater();
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

        operation->deleteLater();
    });

    operation->start();
}

void VCPDeviceInfoHelper::stopRPCSession()
{
    auto *operation = new StopRPCOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
        } else {
            advanceState();
        }

        operation->deleteLater();
    });

    operation->start();
}

void VCPDeviceInfoHelper::closePortAndFinish()
{
    m_serialPort->close();
    finish();
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
