#include "deviceinfohelper.h"

#include <cmath>

#include <QDebug>
#include <QTimer>
#include <QSerialPort>

#include "flipperzero/factoryinfo.h"

#include "flipperzero/cli/stoprpcoperation.h"

#include "flipperzero/cli/storagestatoperation.h"
#include "flipperzero/cli/storageinfooperation.h"
#include "flipperzero/cli/systemdeviceinfooperation.h"

#include "device/stm32wb55.h"

#include "serialinithelper.h"
#include "serialfinder.h"

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
        qDebug() << "Not a Flipper Zero device";
        return nullptr;
    }
}

const DeviceInfo &AbstractDeviceInfoHelper::result() const
{
    return m_deviceInfo;
}

VCPDeviceInfoHelper::VCPDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoHelper(parent),
    m_serialPort(nullptr)
{
    m_deviceInfo.usbInfo = info;
}

void VCPDeviceInfoHelper::nextStateLogic()
{
    if(state() == AbstractDeviceInfoHelper::Ready) {
        setState(VCPDeviceInfoHelper::FindingSerialPort);
        findSerialPort();

    } else if(state() == VCPDeviceInfoHelper::FindingSerialPort) {
        setState(VCPDeviceInfoHelper::InitializingSerialPort);
        initSerialPort();

    } else if(state() == VCPDeviceInfoHelper::InitializingSerialPort) {
        setState(VCPDeviceInfoHelper::FetchingDeviceInfo);
        fetchDeviceInfo();

    } else if(state() == VCPDeviceInfoHelper::FetchingDeviceInfo) {
        setState(VCPDeviceInfoHelper::CheckingSDCard);
        checkSDCard();

    } else if(state() == VCPDeviceInfoHelper::CheckingSDCard) {
        setState(VCPDeviceInfoHelper::CheckingManifest);
        checkManifest();

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
            finishWithError(QStringLiteral("Failed to find a suitable serial port"));

        } else {
            m_deviceInfo.portInfo = portInfo;
            m_deviceInfo.systemLocation = portInfo.systemLocation();

            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::initSerialPort()
{
    auto *helper = new SerialInitHelper(m_deviceInfo.portInfo, this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        if(helper->isError()) {
            finishWithError(QStringLiteral("Failed to initialize serial port: %1").arg(helper->errorString()));
        } else {
            m_serialPort = helper->serialPort();
            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::fetchDeviceInfo()
{
    auto *operation = new SystemDeviceInfoOperation(m_serialPort, this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(QStringLiteral("Failed to get device information: %1").arg(operation->errorString()));
            return;
        }

        m_deviceInfo.name = operation->result(QByteArrayLiteral("hardware_name"));

        m_deviceInfo.bootloader = {
            operation->result(QByteArrayLiteral("bootloader_version")),
            operation->result(QByteArrayLiteral("bootloader_commit")),
            operation->result(QByteArrayLiteral("bootloader_branch")),
            branchToChannelName(operation->result(QByteArrayLiteral("bootloader_branch"))),
            QDateTime::fromString(operation->result(QByteArrayLiteral("bootloader_build_date")), "dd-MM-yyyy").date()
        };

        m_deviceInfo.firmware = {
            operation->result(QByteArrayLiteral("firmware_version")),
            operation->result(QByteArrayLiteral("firmware_commit")),
            operation->result(QByteArrayLiteral("firmware_branch")),
            branchToChannelName(operation->result(QByteArrayLiteral("firmware_branch"))),
            QDateTime::fromString(operation->result(QByteArrayLiteral("firmware_build_date")), "dd-MM-yyyy").date()
        };

        m_deviceInfo.hardware = {
            operation->result(QByteArrayLiteral("hardware_ver")),
            QByteArrayLiteral("f") + operation->result(QByteArrayLiteral("hardware_target")),
            QByteArrayLiteral("b") + operation->result(QByteArrayLiteral("hardware_body")),
            QByteArrayLiteral("c") + operation->result(QByteArrayLiteral("hardware_connect")),
            (HardwareInfo::Color)operation->result(QByteArrayLiteral("hardware_color")).toInt(),
        };

        m_deviceInfo.fusVersion = QStringLiteral("%1.%2.%3").arg(
            operation->result(QByteArrayLiteral("radio_fus_major")),
            operation->result(QByteArrayLiteral("radio_fus_minor")),
            operation->result(QByteArrayLiteral("radio_fus_sub")));

        m_deviceInfo.radioVersion = QStringLiteral("%1.%2.%3").arg(
            operation->result(QByteArrayLiteral("radio_stack_major")),
            operation->result(QByteArrayLiteral("radio_stack_minor")),
            operation->result(QByteArrayLiteral("radio_stack_sub")));

        m_deviceInfo.stackType = operation->result(QByteArrayLiteral("radio_stack_type")).toInt();

        if(m_deviceInfo.name.isEmpty()) {
            finishWithError(QStringLiteral("Failed to read device information: required fields are not present"));
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
            finishWithError(QStringLiteral("Failed to check SD card: %1").arg(operation->errorString()));

        } else if(!operation->isPresent()) {
            m_deviceInfo.storage.isExternalPresent = false;
            m_deviceInfo.storage.isAssetsInstalled = false;

            setState(VCPDeviceInfoHelper::CheckingManifest);
            advanceState();

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
    auto *operation = new StorageStatOperation(m_serialPort, QByteArrayLiteral("/ext/Manifest"), this);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(QStringLiteral("Failed to check resource manifest: %1").arg(operation->errorString()));

        } else {
            m_deviceInfo.storage.isAssetsInstalled = operation->isPresent() && (operation->type() == StorageStatOperation::Type::RegularFile);
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
            finishWithError(QStringLiteral("Failed to stop RPC session: %1").arg(operation->errorString()));
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

const QString &VCPDeviceInfoHelper::branchToChannelName(const QByteArray &branchName)
{
    static const auto DEVELOPMENT = QStringLiteral("development");
    static const auto RELEASE_CANDIDATE = QStringLiteral("release-candidate");
    static const auto RELEASE = QStringLiteral("release");

    if(branchName == QByteArrayLiteral("dev")) {
        return DEVELOPMENT;
    } else if(branchName.contains(QByteArrayLiteral("-rc"))) {
        return RELEASE_CANDIDATE;
    } else {
        return RELEASE;
    }
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
