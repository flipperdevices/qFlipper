#include "deviceinfohelper.h"

#include <cmath>

#include <QDebug>
#include <QTimer>
#include <QRegExp>
#include <QSerialPort>
#include <QLoggingCategory>

#include "flipperzero/factoryinfo.h"
#include "flipperzero/protobufsession.h"

#include "flipperzero/rpc/stoprpcoperation.h"
#include "flipperzero/rpc/storageinfooperation.h"
#include "flipperzero/rpc/storagestatoperation.h"
#include "flipperzero/rpc/propertygetoperation.h"
#include "flipperzero/rpc/systemdeviceinfooperation.h"
#include "flipperzero/rpc/systemgetdatetimeoperation.h"
#include "flipperzero/rpc/systemsetdatetimeoperation.h"
#include "flipperzero/rpc/systemprotobufversionoperation.h"

#include "device/stm32wb55.h"
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
        setState(VCPDeviceInfoHelper::StartingRPCSession);
        startRPCSession();

    } else if(state() == VCPDeviceInfoHelper::StartingRPCSession) {
        setState(VCPDeviceInfoHelper::FetchingProtobufVersion);
        fetchProtobufVersion();

    } else if(state() == VCPDeviceInfoHelper::FetchingProtobufVersion) {
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
    }
}

void VCPDeviceInfoHelper::findSerialPort()
{
    auto *finder = new SerialFinder(m_deviceInfo.usbInfo.serialNumber(), this);

    connect(finder, &SerialFinder::finished, this, [=](const QSerialPortInfo &portInfo) {
        if(portInfo.isNull()) {
            finishWithError(BackendError::SerialAccessError, QStringLiteral("Failed to find a suitable serial port"));

        } else {
            qCDebug(CATEGORY_DEBUG).noquote() << "Using  serial port" << portInfo.serialNumber() << "at" << portInfo.systemLocation();
            m_deviceInfo.portInfo = portInfo;
            m_deviceInfo.systemLocation = portInfo.systemLocation();

            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::startRPCSession()
{
    m_rpc = new ProtobufSession(m_deviceInfo.portInfo, this);
    connect(m_rpc, &ProtobufSession::sessionStateChanged, this, &VCPDeviceInfoHelper::onSessionStatusChanged);
    m_rpc->startSession();
}

void VCPDeviceInfoHelper::fetchProtobufVersion()
{
    auto *operation = m_rpc->systemProtobufVersion();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to get protobuf version: %1").arg(operation->errorString()));
            return;
        }

        m_deviceInfo.protobuf = {
            operation->versionMajor(),
            operation->versionMinor()
        };

        qCDebug(CATEGORY_DEBUG).noquote() << QStringLiteral("Detected protobuf version: %1.%2").arg(operation->versionMajor()).arg(operation->versionMinor());

        advanceState();
    });
}

void VCPDeviceInfoHelper::fetchDeviceInfo()
{
    const auto &protobuf = m_deviceInfo.protobuf;

    if((protobuf.versionMajor > 0) || (protobuf.versionMinor >= 14)) {
        fetchDeviceInfoProperty();
    } else {
        fetchDeviceInfoLegacy();
    }
}

void VCPDeviceInfoHelper::fetchDeviceInfoLegacy()
{
    auto *operation = m_rpc->systemDeviceInfo();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to get device information: %1").arg(operation->errorString()));
            return;
        }

        m_deviceInfo.name = operation->value(QByteArrayLiteral("hardware_name"));

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
            (Color)operation->value(QByteArrayLiteral("hardware_color")).toInt(),
            (Region)operation->value(QByteArrayLiteral("hardware_region")).toInt(),
        };

        if(operation->value(QByteArrayLiteral("radio_alive")) == QByteArrayLiteral("true")) {
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

void VCPDeviceInfoHelper::fetchDeviceInfoProperty()
{
    auto *operation = m_rpc->propertyGet(QByteArrayLiteral("devinfo"));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to get device information: %1").arg(operation->errorString()));
            return;
        }

        m_deviceInfo.name = operation->value(QByteArrayLiteral("hardware.name"));

        m_deviceInfo.firmware = {
            operation->value(QByteArrayLiteral("firmware.version")),
            operation->value(QByteArrayLiteral("firmware.commit.hash")),
            operation->value(QByteArrayLiteral("firmware.branch.name")),
            branchToChannelName(operation->value(QByteArrayLiteral("firmware.branch.name"))),
            QDateTime::fromString(operation->value(QByteArrayLiteral("firmware.build.date")), "dd-MM-yyyy").date()
        };

        m_deviceInfo.hardware = {
            operation->value(QByteArrayLiteral("hardware.ver")),
            QByteArrayLiteral("f") + operation->value(QByteArrayLiteral("hardware.target")),
            QByteArrayLiteral("b") + operation->value(QByteArrayLiteral("hardware.body")),
            QByteArrayLiteral("c") + operation->value(QByteArrayLiteral("hardware.connect")),
            (Color)operation->value(QByteArrayLiteral("hardware.color")).toInt(),
            (Region)operation->value(QByteArrayLiteral("hardware.region.builtin")).toInt(),
        };

        if(operation->value(QByteArrayLiteral("radio.alive")) == QByteArrayLiteral("true")) {
            m_deviceInfo.fusVersion = QStringLiteral("%1.%2.%3").arg(
                operation->value(QByteArrayLiteral("radio.fus.major")),
                operation->value(QByteArrayLiteral("radio.fus.minor")),
                operation->value(QByteArrayLiteral("radio.fus.sub")));

            m_deviceInfo.radioVersion = QStringLiteral("%1.%2.%3").arg(
                operation->value(QByteArrayLiteral("radio.stack.major")),
                operation->value(QByteArrayLiteral("radio.stack.minor")),
                operation->value(QByteArrayLiteral("radio.stack.sub")));

            m_deviceInfo.stackType = operation->value(QByteArrayLiteral("radio.stack.type")).toInt();
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
    auto *operation = m_rpc->storageInfo(QByteArrayLiteral("/ext"));

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
    auto *operation = m_rpc->storageStat(QByteArrayLiteral("/ext/Manifest"));

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
    auto *operation = m_rpc->getDateTime();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check device time: %1").arg(operation->errorString()));

        } else {
            const auto timeSkew = QDateTime::currentDateTime().msecsTo(operation->dateTime());
            qCDebug(CATEGORY_DEBUG) << "Flipper time skew is" << timeSkew << "milliseconds";

            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::syncTime()
{
    auto *operation = m_rpc->setDateTime(QDateTime::currentDateTime());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to set device time: %1").arg(operation->errorString()));
        } else {
            advanceState();
        }
    });
}

void VCPDeviceInfoHelper::stopRPCSession()
{
    m_rpc->stopSession();
}

void VCPDeviceInfoHelper::onSessionStatusChanged()
{
    if(m_rpc->isError()) {
        finishWithError(m_rpc->error(), QStringLiteral("Protobuf session error: %1").arg(m_rpc->errorString()));
    } else if(state() == VCPDeviceInfoHelper::StartingRPCSession && m_rpc->isSessionUp()) {
        advanceState();
    } else if(state() == VCPDeviceInfoHelper::StoppingRPCSession && !m_rpc->isSessionUp()) {
        finish();
    }
}

const QString &VCPDeviceInfoHelper::branchToChannelName(const QByteArray &branchName)
{
    static const auto DEVELOPMENT = QStringLiteral("development");
    static const auto RELEASE_CANDIDATE = QStringLiteral("release-candidate");
    static const auto RELEASE = QStringLiteral("release");
    static const auto CUSTOM = QStringLiteral("custom");

    const QRegExp validVersion(QStringLiteral("^\\d+\\.\\d+\\.\\d+(-rc)?$"));

    if(validVersion.exactMatch(branchName)) {
        if(validVersion.cap(1).isEmpty()) {
            return RELEASE;
        } else {
            return RELEASE_CANDIDATE;
        }

    } else if(branchName == QByteArrayLiteral("dev")) {
        return DEVELOPMENT;
    } else {
        return CUSTOM;
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
    m_deviceInfo.hardware.color = factoryInfo.color();
    m_deviceInfo.hardware.region = factoryInfo.region();

    finish();
}
