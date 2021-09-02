#include "flipperzero.h"

#include <QTime>
#include <QThread>
#include <QBuffer>
#include <QSerialPort>

#include "recoverycontroller.h"
#include "deviceinfofetcher.h"
#include "remotecontroller.h"
#include "serialfinder.h"
#include "factoryinfo.h"
#include "dfusefile.h"
#include "macros.h"

#include "device/stm32wb55.h"

#define to_hex_str(num) (QString::number(num, 16))

/* ----------------------------------------------------------------------------------------------------------------------------------
 * FUS operations are based on the info from AN5185
 * https://www.st.com/resource/en/application_note/dm00513965-st-firmware-upgrade-services-for-stm32wb-series-stmicroelectronics.pdf
 * ---------------------------------------------------------------------------------------------------------------------------------- */

namespace Flipper {

using namespace Zero;
using namespace STM32;
using namespace WB55;

FlipperZero::FlipperZero(const Zero::DeviceInfo &info, QObject *parent):
    QObject(parent),

    m_isPersistent(false),
    m_isOnline(true),
    m_isError(false),

    m_deviceInfo(info),

    m_progress(0),
    m_remote(nullptr),
    m_recovery(nullptr)
{
    initControllers();
}

FlipperZero::~FlipperZero()
{
    setOnline(false);
}

void FlipperZero::reset(const Zero::DeviceInfo &info)
{
    setDeviceInfo(info);
    initControllers();

    setError(QStringLiteral("No error"), false);
    setProgress(0);
    setOnline(true);
}

void FlipperZero::setDeviceInfo(const Zero::DeviceInfo &info)
{
    // Not checking the huge structure for equality
    m_deviceInfo = info;
    emit deviceInfoChanged();
}

void FlipperZero::setPersistent(bool set)
{
    if(set == m_isPersistent) {
        return;
    }

    m_isPersistent = set;
    emit isPersistentChanged();
}

void FlipperZero::setOnline(bool set)
{
    if(set == m_isOnline) {
        return;
    }

    m_isOnline = set;
    emit isOnlineChanged();
}

void FlipperZero::setError(const QString &msg, bool set)
{
    if(set == m_isError) {
        return;
    }

    m_isError = set;
    emit isErrorChanged();

    if(!msg.isEmpty()) {
        setStatusMessage(msg);
    }
}

bool FlipperZero::isPersistent() const
{
    return m_isPersistent;
}

bool FlipperZero::isOnline() const
{
    return m_isOnline;
}

bool FlipperZero::isError() const
{
    return m_isError;
}

bool FlipperZero::bootToDFU()
{
    statusFeedback("Entering DFU bootloader mode...");

    auto *serialPort = new QSerialPort(m_deviceInfo.serialInfo, this);

    const auto portSuccess = serialPort->open(QIODevice::WriteOnly) && serialPort->setDataTerminalReady(true) &&
                            (serialPort->write(QByteArrayLiteral("\rdfu\r")) > 0);

    // TODO: Is it necessary here? Why was this added?
    auto flushTries = 30;

    while(--flushTries && !serialPort->flush()) {
        info_msg("Serial port flush failure, retrying...");
        QThread::msleep(15);
    }

    serialPort->close();

    const auto success = portSuccess && flushTries;

    if(!success) {
        errorFeedback("Can't detach the device: Failed to reset in DFU mode");
        error_msg(QString("Serial port status: %1").arg(serialPort->errorString()));
    }

    serialPort->deleteLater();

    return success;
}

bool FlipperZero::reboot()
{
    statusFeedback("Booting the device up...");

    STM32WB55 device(m_deviceInfo.usbInfo);
    const auto success = device.beginTransaction() && device.leave();

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    if(!success) {
        errorFeedback("Failed to leave DFU mode.");
    }

    return success;
}

bool FlipperZero::setBootMode(BootMode mode)
{
    const auto msg = (mode == BootMode::Normal) ? "Booting the device up..." : "Setting device to DFU boot mode...";
    statusFeedback(msg);

    STM32WB55 device(m_deviceInfo.usbInfo);

    if(!device.beginTransaction()) {
        errorFeedback("Can't set boot mode: Failed to initiate transaction.");
        return false;
    }

    auto ob = device.optionBytes();

    if(!ob.isValid()) {
        errorFeedback("Can't set boot mode: Failed to read option bytes.");
        return false;
    }

    ob.setValue("nBOOT0", mode == BootMode::Normal);
    ob.setValue("nSWBOOT0", mode == BootMode::Normal);

    const auto success = device.setOptionBytes(ob);

    if(!success) {
        errorFeedback("Cant' set boot mode: Failed to set option bytes");
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    return success;
}

FlipperZero::WirelessStatus FlipperZero::wirelessStatus()
{
    info_msg("Getting Co-Processor (Wireless) status...");

    STM32WB55 device(m_deviceInfo.usbInfo);

    if(!device.beginTransaction()) {
        info_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    const auto state = device.FUSGetState();
    if(!state.isValid()) {
        info_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    if(!device.endTransaction()) {
        info_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    const auto status = state.status();
    const auto error = state.error();

    info_msg(QStringLiteral("Current FUS state: %1, %2.").arg(state.statusString(), state.errorString()));

    if((status == FUSState::Idle) && (error == FUSState::NoError)) {
        return WirelessStatus::FUSRunning;
    } else if(status == FUSState::ErrorOccured) {
         if(error == FUSState::NotRunning)
            return WirelessStatus::WSRunning;
         else
             return WirelessStatus::ErrorOccured;

    } else {
        return WirelessStatus::UnhandledState;
    }
}

bool FlipperZero::startFUS()
{
    statusFeedback("Starting firmware upgrade service (FUS)...");

    STM32WB55 device(m_deviceInfo.usbInfo);

    if(!device.beginTransaction()) {
        errorFeedback("Can't start FUS: Failed to initiate transaction.");
        return false;
    }

    auto state = device.FUSGetState();
    auto success = state.isValid();

    if(!success) {
        errorFeedback("Can't start FUS: Failed to get FUS state.");

    } else if((state.status() == FUSState::Idle) && (state.error() == FUSState::NoError)) {
        info_msg("FUS is already RUNNING, rebooting for consistency...");
        success = device.leave();

    } else if((state.status() == FUSState::ErrorOccured) && (state.error() == FUSState::NotRunning)) {
        info_msg(QString("FUS appears NOT to be running: %1, %2.").arg(state.statusString(), state.errorString()));

        // Send a second GET_STATE to actually start FUS
        begin_ignore_block();
        state = device.FUSGetState();
        end_ignore_block();

    } else {
        error_msg("Unexpected FUS state.");
        success = false;
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    // At this point, there is no way to know whether FUS has actually started, but things are looking as expected.
    return success;
}

// TODO: check status to see if the wireless stack is present at all
bool FlipperZero::startWirelessStack()
{
    statusFeedback("Attempting to start the Wireless Stack...");

    STM32WB55 device(m_deviceInfo.usbInfo);

    auto success = device.beginTransaction() && device.FUSStartWirelessStack();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(!success) {
        errorFeedback("Failed to start wireless stack.");
    }

    return success;
}

bool FlipperZero::deleteWirelessStack()
{
    statusFeedback("Deleting old co-processor firmware...");

    STM32WB55 device(m_deviceInfo.usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwDelete() && device.endTransaction();

    if(!success) {
        errorFeedback("Can't delete old co-processor firmware: Failed to initiate wireless stack firmware removal.");
    }

    return success;
}

bool FlipperZero::downloadFirmware(QIODevice *file)
{
    if(!file->open(QIODevice::ReadOnly)) {
        errorFeedback("Can't download firmware: Failed to open the file.");
        return false;

    } else if(file->bytesAvailable() <= 0) {
        errorFeedback("Can't download firmware: The file is empty.");
        return false;

    } else {
        statusFeedback("Downloading the firmware, please wait...");
    }

    DfuseFile fw(file);
    DfuseDevice dev(m_deviceInfo.usbInfo);

    file->close();

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) && dev.endTransaction();

    if(!success) {
        errorFeedback("Can't download firmware: An error has occured during the operation.");
    }

    return success;
}

bool FlipperZero::downloadWirelessStack(QIODevice *file, uint32_t addr)
{
    info_msg("Attempting to download CO-PROCESSOR firmware image...");

    if(!file->open(QIODevice::ReadOnly)) {
        errorFeedback("Can't download co-processor firmware image: Failed to open file.");
        return false;

    } else if(!file->bytesAvailable()) {
        errorFeedback("Can't download co-processor firmware image: File is empty.");
        return false;

    } else {
        statusFeedback("Downloading co-processor firmware image...");
    }

    STM32WB55 device(m_deviceInfo.usbInfo);

    if(!device.beginTransaction()) {
        errorFeedback("Can't download co-processor firmware image: Failed to initiate transaction.");
        return false;
    }

    if(!addr) {
        const auto ob = device.optionBytes();

        if(!ob.isValid()) {
            errorFeedback("Can't download co-processor firmware image: Failed to read Option Bytes.");
            return false;
        }

        const auto origin = device.partitionOrigin((uint8_t)STM32WB55::Partition::Flash);
        const auto pageSize = (uint32_t)0x1000; // TODO: do not hardcode page size

        addr = (origin + (pageSize * ob.value("SFSA")) - file->bytesAvailable()) & (~(pageSize - 1));

        info_msg(QString("SFSA value is 0x%1").arg(QString::number(ob.value("SFSA"), 16)));
        info_msg(QString("Target address for co-processor firmware image is 0x%1").arg(QString::number(addr, 16)));

    } else {
        info_msg(QString("Target address for co-processor firmware image has been OVERRIDDEN to 0x%1").arg(QString::number(addr, 16)));
    }

    connect(&device, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    bool success;

    if(!(success = device.erase(addr, file->bytesAvailable()))) {
        errorFeedback("Can't download co-processor firmware image: Failed to erase the internal memory.");
    } else if(!(success = device.download(file, addr, 0))) {
        errorFeedback("Can't download co-processor firmware image: Failed to write the internal memory.");
    } else if(!(success = device.endTransaction())) {
        errorFeedback("Can't download co-processor firmware image: Failed to end transaction.");
    } else {}

    file->close();

    return success;
}

bool FlipperZero::upgradeWirelessStack()
{
    info_msg("Sending FW_UPGRADE command...");

    STM32WB55 device(m_deviceInfo.usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwUpgrade();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(!success) {
        errorFeedback("Can't upgrade Co-Processor firmware: Failed to initiate installation.");
    } else {
        statusFeedback("Upgrading Co-Processor firmware, please wait...");
    }

    return success;
}

bool FlipperZero::downloadOptionBytes(QIODevice *file)
{
    statusFeedback("Fixing Option Bytes...");

    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open file for reading");
    const OptionBytes loaded(file);
    file->close();

    check_return_bool(loaded.isValid(), "Failed to load option bytes from file");

    STM32WB55 device(m_deviceInfo.usbInfo);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    const OptionBytes actual = device.optionBytes();

    const auto diff = actual.compare(loaded);

    bool success = false;

    if(diff.isEmpty()) {
        info_msg("Option Bytes OK");

        success = device.leave();

        if(!success) {
            errorFeedback("Can't set boot mode: Failed to leave the DFU mode.");
        }

    } else {
        for(auto it = diff.constKeyValueBegin(); it != diff.constKeyValueEnd(); ++it) {
            info_msg(QString("Option Bytes mismatch @%1: this: 0x%2, other: 0x%3")
                     .arg((*it).first, to_hex_str(actual.value((*it).first)), to_hex_str((*it).second)));
        }

        info_msg("Writing corrected Option Bytes...");

        success = device.setOptionBytes(actual.corrected(diff));

        if(!success) {
            errorFeedback("Can't set boot mode: Failed to set option bytes");
        }
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    if(success) {
        statusFeedback("Booting up the device...");
    }

    return success;
}

const QString &FlipperZero::name() const
{
    return m_deviceInfo.name;
}

const QString &FlipperZero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &FlipperZero::target() const
{
    return m_deviceInfo.target;
}

const QString &FlipperZero::version() const
{
    return m_deviceInfo.firmware.version;
}

const QString &FlipperZero::statusMessage() const
{
    return m_statusMessage;
}

double FlipperZero::progress() const
{
    return m_progress;
}

const USBDeviceInfo &FlipperZero::usbInfo() const
{
    return m_deviceInfo.usbInfo;
}

const DeviceInfo &FlipperZero::deviceInfo() const
{
    return m_deviceInfo;
}

bool FlipperZero::isDFU() const
{
    return m_deviceInfo.usbInfo.productID() == 0xdf11;
}

Flipper::Zero::RemoteController *FlipperZero::remote() const
{
    return m_remote;
}

RecoveryController *FlipperZero::recovery() const
{
    return m_recovery;
}

void FlipperZero::setName(const QString &name)
{
    if(m_deviceInfo.name == name) {
        return;
    }

    m_deviceInfo.name = name;
    emit deviceInfoChanged();
}

void FlipperZero::setTarget(const QString &target)
{
    if(m_deviceInfo.target == target) {
        return;
    }

    m_deviceInfo.target = target;
    emit deviceInfoChanged();
}

void FlipperZero::setVersion(const QString &version)
{
    if(m_deviceInfo.firmware.version == version) {
        return;
    }

    m_deviceInfo.firmware.version = version;
    emit deviceInfoChanged();
}

void FlipperZero::setStatusMessage(const QString &message)
{
    if(m_statusMessage != message) {
        emit statusMessageChanged(m_statusMessage = message);
    }
}

void FlipperZero::setProgress(double progress)
{
    if(!qFuzzyCompare(m_progress, progress)) {
        emit progressChanged(m_progress = progress);
    }
}

void FlipperZero::initControllers()
{
    if(m_remote) {
       m_remote->deleteLater();
       m_remote = nullptr;
    }

    if(m_recovery) {
       m_recovery->deleteLater();
       m_recovery = nullptr;
    }

    if(isDFU()) {
        // TODO: connect the necessary signals
        m_recovery = new RecoveryController(m_deviceInfo.usbInfo, this);

    } else {
        m_remote = new RemoteController(m_deviceInfo.serialInfo, this);
    }
}

void FlipperZero::statusFeedback(const char *msg)
{
    info_msg(msg);
    setStatusMessage(tr(msg));
}

void FlipperZero::errorFeedback(const char *msg)
{
    error_msg(msg);
    setError(tr("<b>ERROR:</b> ") + tr(msg));
}

}
