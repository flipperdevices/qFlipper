#include "deviceinfohelper.h"

#include <cmath>

#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QLoggingCategory>

#include "flipperzero/factoryinfo.h"
#include "flipperzero/protobufsession.h"

#include "flipperzero/rpc/stoprpcoperation.h"
#include "flipperzero/rpc/storageinfooperation.h"
#include "flipperzero/rpc/storagestatoperation.h"
#include "flipperzero/rpc/systemdeviceinfooperation.h"
#include "flipperzero/rpc/systemgetdatetimeoperation.h"
#include "flipperzero/rpc/systemsetdatetimeoperation.h"

#include "device/stm32wb55.h"

#include "serialinithelper.h"
#include "serialfinder.h"

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

using namespace Flipper;
using namespace Zero;

AbstractDeviceInfoHelper::AbstractDeviceInfoHelper(QObject *parent):
    AbstractOperationHelper(parent),
    m_deviceInfo({})
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
        qCDebug(CATEGORY_DEBUG) << "Not a Flipper Zero device";
        return nullptr;
    }
}

const DeviceInfo &AbstractDeviceInfoHelper::result() const
{
    return m_deviceInfo;
}

VCPDeviceInfoHelper::VCPDeviceInfoHelper(const USBDeviceInfo &info, QObject *parent):
    AbstractDeviceInfoHelper(parent)
{
    m_deviceInfo.usbInfo = info;
}

void VCPDeviceInfoHelper::nextStateLogic()
{
    if(state() == AbstractDeviceInfoHelper::Ready) {
        setState(VCPDeviceInfoHelper::FindingSerialPort);
        findSerialPort();

    } else if(state() == VCPDeviceInfoHelper::FindingSerialPort) {
        setState(VCPDeviceInfoHelper::StartingProtobufSession);
        startProtobufSession();

    } else if(state() == VCPDeviceInfoHelper::StartingProtobufSession) {
        setState(VCPDeviceInfoHelper::FetchingDeviceInfo);
        fetchDeviceInfo();

    } else if(state() == VCPDeviceInfoHelper::FetchingDeviceInfo) {
        setState(VCPDeviceInfoHelper::CheckingSDCard);
        checkSDCard();

    } else if(state() == VCPDeviceInfoHelper::CheckingSDCard) {
        setState(VCPDeviceInfoHelper::CheckingManifest);
        checkManifest();

    } else if(state() == VCPDeviceInfoHelper::CheckingManifest) {
        setState(VCPDeviceInfoHelper::GettingTimeSkew);
        getTimeSkew();

    } else if(state() == VCPDeviceInfoHelper::GettingTimeSkew) {
        setState(VCPDeviceInfoHelper::SyncingTime);
        syncTime();

    } else if(state() == VCPDeviceInfoHelper::SyncingTime) {
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
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to find a suitable serial port"));

        } else {
            m_deviceInfo.portInfo = portInfo;
            m_deviceInfo.systemLocation = portInfo.systemLocation();

            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::startProtobufSession()
{
    m_session = new ProtobufSession(m_deviceInfo.portInfo, this);
    connect(m_session, &ProtobufSession::sessionStateChanged, this, &VCPDeviceInfoHelper::onProtobufSessionStateChanged);
}

void VCPDeviceInfoHelper::fetchDeviceInfo()
{
    auto *operation = m_session->systemDeviceInfo();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to get device information: %1").arg(operation->errorString()));
            return;
        }

        m_deviceInfo.name = operation->value(QByteArrayLiteral("hardware_name"));

        m_deviceInfo.bootloader = {
            operation->value(QByteArrayLiteral("bootloader_version")),
            operation->value(QByteArrayLiteral("bootloader_commit")),
            operation->value(QByteArrayLiteral("bootloader_branch")),
            branchToChannelName(operation->value(QByteArrayLiteral("bootloader_branch"))),
            QDateTime::fromString(operation->value(QByteArrayLiteral("bootloader_build_date")), "dd-MM-yyyy").date()
        };

        m_deviceInfo.firmware = {
            operation->value(QByteArrayLiteral("firmware_version")),
            operation->value(QByteArrayLiteral("firmware_commit")),
            operation->value(QByteArrayLiteral("firmware_branch")),
            branchToChannelName(operation->value(QByteArrayLiteral("firmware_branch"))),
            QDateTime::fromString(operation->value(QByteArrayLiteral("firmware_build_date")), "dd-MM-yyyy").date()
        };

        m_deviceInfo.hardware = {
            operation->value(QByteArrayLiteral("hardware_ver")),
            QByteArrayLiteral("f") + operation->value(QByteArrayLiteral("hardware_target")),
            QByteArrayLiteral("b") + operation->value(QByteArrayLiteral("hardware_body")),
            QByteArrayLiteral("c") + operation->value(QByteArrayLiteral("hardware_connect")),
            (HardwareInfo::Color)operation->value(QByteArrayLiteral("hardware_color")).toInt(),
        };

        if(operation->value(QByteArray("radio_alive")) == QByteArrayLiteral("true")) {
            m_deviceInfo.fusVersion = QStringLiteral("%1.%2.%3").arg(
                operation->value(QByteArrayLiteral("radio_fus_major")),
                operation->value(QByteArrayLiteral("radio_fus_minor")),
                operation->value(QByteArrayLiteral("radio_fus_sub")));

            m_deviceInfo.radioVersion = QStringLiteral("%1.%2.%3").arg(
                operation->value(QByteArrayLiteral("radio_stack_major")),
                operation->value(QByteArrayLiteral("radio_stack_minor")),
                operation->value(QByteArrayLiteral("radio_stack_sub")));

            m_deviceInfo.stackType = operation->value(QByteArrayLiteral("radio_stack_type")).toInt();
        }

        if(m_deviceInfo.name.isEmpty()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to read device information: required fields are not present"));
        } else {
            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::checkSDCard()
{
    auto *operation = m_session->storageInfo(QByteArrayLiteral("/ext"));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check SD card: %1").arg(operation->errorString()));

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
    });
}

void VCPDeviceInfoHelper::checkManifest()
{
    auto *operation = m_session->storageStat(QByteArrayLiteral("/ext/Manifest"));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check resource manifest: %1").arg(operation->errorString()));

        } else {
            m_deviceInfo.storage.isAssetsInstalled = operation->hasFile() && (operation->type() == StorageStatOperation::RegularFile);
            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::getTimeSkew()
{
//    auto *operation = new SystemGetDateTimeOperation(m_serialPort, this);
//    operation->start();

//    connect(operation, &AbstractOperation::finished, this, [=]() {
//        if(operation->isError()) {
//            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check device time: %1").arg(operation->errorString()));

//        } else {
//            const auto timeSkew = QDateTime::currentDateTime().msecsTo(operation->dateTime());
//            qCDebug(CATEGORY_DEBUG) << "Flipper time skew is" << timeSkew << "milliseconds";

//            advanceState();
//        }

//        operation->deleteLater();
//    });
    finishWithError(BackendError::UnknownError, QStringLiteral("Not implemented"));
}

void VCPDeviceInfoHelper::syncTime()
{
//    auto *operation = new SystemSetDateTimeOperation(m_serialPort, QDateTime::currentDateTime(), this);
//    operation->start();

//    connect(operation, &AbstractOperation::finished, this, [=]() {
//        if(operation->isError()) {
//            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to set device time: %1").arg(operation->errorString()));
//        } else {
//            advanceState();
//        }

//        operation->deleteLater();
//    });
    finishWithError(BackendError::UnknownError, QStringLiteral("Not implemented"));
}

void VCPDeviceInfoHelper::stopRPCSession()
{
//    auto *operation = new StopRPCOperation(m_serialPort, this);
//    connect(operation, &AbstractOperation::finished, this, [=]() {
//        if(operation->isError()) {
//            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to stop RPC session: %1").arg(operation->errorString()));
//        } else {
//            advanceState();
//        }

//        operation->deleteLater();
//    });

//    operation->start();
    finishWithError(BackendError::UnknownError, QStringLiteral("Not implemented"));
}

void VCPDeviceInfoHelper::closePortAndFinish()
{
}

void VCPDeviceInfoHelper::onProtobufSessionStateChanged()
{
    if(m_session->isError()) {
        finishWithError(m_session->error(), QStringLiteral("Protobuf session error: %1").arg(m_session->errorString()));

    } else if(state() == VCPDeviceInfoHelper::StartingProtobufSession && m_session->sessionState() == ProtobufSession::Idle) {
        advanceState();

    } else if(state() == VCPDeviceInfoHelper::StoppingRPCSession && m_session->sessionState() == ProtobufSession::Stopped) {
        qCDebug(CATEGORY_DEBUG) << "RPC session stopped successfully.";
        advanceState();
    }
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
        finishWithError(BackendError::RecoveryAccessError, QStringLiteral("Failed to initiate transaction"));
        return;
    }

    const FactoryInfo factoryInfo(device.OTPData(FactoryInfo::size()));

    if(!device.endTransaction()) {
        finishWithError(BackendError::RecoveryAccessError, QStringLiteral("Failed to end transaction"));
        return;
    }

    if(!factoryInfo.isValid()) {
        finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to read device factory information"));
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
