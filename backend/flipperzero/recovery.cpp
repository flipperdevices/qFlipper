#include "recovery.h"

#include "devicestate.h"
#include "dfusefile.h"
#include "debug.h"

#include "device/stm32wb55.h"

using namespace Flipper;
using namespace Zero;

using namespace STM32;
using namespace WB55;

/* ----------------------------------------------------------------------------------------------------------------------------------
 * FUS operations are based on the info from AN5185
 * https://www.st.com/resource/en/application_note/dm00513965-st-firmware-upgrade-services-for-stm32wb-series-stmicroelectronics.pdf
 * ---------------------------------------------------------------------------------------------------------------------------------- */

#define to_hex_str(num) (QString::number(num, 16))

Recovery::Recovery(DeviceState *deviceState, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState)
{}

Recovery::~Recovery()
{}

DeviceState *Recovery::deviceState() const
{
    return m_deviceState;
}

bool Recovery::exitRecoveryMode()
{
    m_deviceState->setStatusString(QStringLiteral("Exiting recovery mode..."));

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);
    const auto success = device.beginTransaction() && device.leave();

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    if(!success) {
        setErrorString("Failed to exit recovery mode");
    }

    return success;
}

bool Recovery::setBootMode(BootMode mode)
{
    const auto msg = (mode == BootMode::Normal) ?
               QStringLiteral("Setting OS boot mode...") :
               QStringLiteral("Setting Recovery boot mode...");

    m_deviceState->setStatusString(msg);

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    if(!device.beginTransaction()) {
        setErrorString("Can't set boot mode: Failed to initiate transaction.");
        return false;
    }

    auto ob = device.optionBytes();

    if(!ob.isValid()) {
        setErrorString("Can't set boot mode: Failed to read option bytes.");
        return false;
    }

    ob.setValue("nBOOT0", mode == BootMode::Normal);
    ob.setValue("nSWBOOT0", mode == BootMode::Normal);

    const auto success = device.setOptionBytes(ob);

    if(!success) {
        setErrorString("Can't set boot mode: Failed to set option bytes");
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    return success;
}

Recovery::WirelessStatus Recovery::wirelessStatus()
{
    debug_msg("Getting Co-Processor (Wireless) status...");

    if(!m_deviceState->isOnline()) {
        debug_msg("Failed to get FUS status. The device is offline at the moment.");
        return WirelessStatus::Invalid;
    }

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    if(!device.beginTransaction()) {
        debug_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    const auto state = device.FUSGetState();
    if(!state.isValid()) {
        debug_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    if(!device.endTransaction()) {
        debug_msg("Failed to get FUS status. This is normal if the device has just rebooted.");
        return WirelessStatus::Invalid;
    }

    const auto status = state.status();
    const auto error = state.error();

    debug_msg(QStringLiteral("Current FUS state: %1, %2.").arg(state.statusString(), state.errorString()));

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

bool Recovery::startFUS()
{
    m_deviceState->setStatusString("Starting firmware upgrade service (FUS)...");

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    if(!device.beginTransaction()) {
        setErrorString("Can't start FUS: Failed to initiate transaction.");
        return false;
    }

    auto state = device.FUSGetState();
    auto success = state.isValid();

    if(!success) {
        setErrorString("Can't start FUS: Failed to get FUS state.");

    } else if((state.status() == FUSState::Idle) && (state.error() == FUSState::NoError)) {
        debug_msg("FUS is already RUNNING, rebooting for consistency...");
        success = device.leave();

    } else if((state.status() == FUSState::ErrorOccured) && (state.error() == FUSState::NotRunning)) {
        debug_msg(QString("FUS appears NOT to be running: %1, %2.").arg(state.statusString(), state.errorString()));

        // Send a second GET_STATE to actually start FUS
        begin_ignore_block();
        state = device.FUSGetState();
        end_ignore_block();

    } else {
        setErrorString("Can't start FUS: Unexpected FUS state.");
        success = false;
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    // At this point, there is no way to know whether FUS has actually started, but things are looking as expected.
    return success;
}

// TODO: check status to see if the wireless stack is present at all
bool Recovery::startWirelessStack()
{
    m_deviceState->setStatusString("Attempting to start the Wireless Stack...");

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    auto success = device.beginTransaction() && device.FUSStartWirelessStack();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(!success) {
        setErrorString("Failed to start wireless stack.");
    }

    return success;
}

bool Recovery::deleteWirelessStack()
{
    m_deviceState->setStatusString("Deleting old co-processor firmware...");

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwDelete() && device.endTransaction();

    if(!success) {
        setErrorString("Can't delete old co-processor firmware: Failed to initiate wireless stack firmware removal.");
    }

    return success;
}

bool Recovery::downloadFirmware(QIODevice *file)
{
    if(!file->open(QIODevice::ReadOnly)) {
        setErrorString("Can't flash firmware: Failed to open the file.");
        return false;

    } else if(file->bytesAvailable() <= 0) {
        setErrorString("Can't flash firmware: The file is empty.");
        return false;

    } else {
        m_deviceState->setStatusString("Flashing firmware...");
    }

    DfuseFile fw(file);
    DfuseDevice dev(m_deviceState->deviceInfo().usbInfo);

    file->close();

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        m_deviceState->setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) && dev.endTransaction();

    if(!success) {
        setErrorString("Can't flash firmware: An error has occurred during the operation.");
    }

    return success;
}

bool Recovery::downloadWirelessStack(QIODevice *file, uint32_t addr)
{
    debug_msg("Attempting to flash CO-PROCESSOR firmware image...");

    if(!file->open(QIODevice::ReadOnly)) {
        setErrorString("Can't flash co-processor firmware image: Failed to open file.");
        return false;

    } else if(!file->bytesAvailable()) {
        setErrorString("Can't flash co-processor firmware image: File is empty.");
        return false;

    } else {
        m_deviceState->setStatusString("Flashing co-processor firmware image...");
    }

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    if(!device.beginTransaction()) {
        setErrorString("Can't flash co-processor firmware image: Failed to initiate transaction.");
        return false;
    }

    if(!addr) {
        const auto ob = device.optionBytes();

        if(!ob.isValid()) {
            setErrorString("Can't flash co-processor firmware image: Failed to read Option Bytes.");
            return false;
        }

        const auto origin = device.partitionOrigin((uint8_t)STM32WB55::Partition::Flash);
        const auto pageSize = (uint32_t)0x1000; // TODO: do not hardcode page size

        addr = (origin + (pageSize * ob.value("SFSA")) - file->bytesAvailable()) & (~(pageSize - 1));

        debug_msg(QString("SFSA value is 0x%1").arg(QString::number(ob.value("SFSA"), 16)));
        debug_msg(QString("Target address for co-processor firmware image is 0x%1").arg(QString::number(addr, 16)));

    } else {
        debug_msg(QString("Target address for co-processor firmware image has been OVERRIDDEN to 0x%1").arg(QString::number(addr, 16)));
    }

    connect(&device, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        m_deviceState->setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    bool success;

    if(!(success = device.erase(addr, file->bytesAvailable()))) {
        setErrorString("Can't flash co-processor firmware image: Failed to erase the internal memory.");
    } else if(!(success = device.download(file, addr, 0))) {
        setErrorString("Can't flash co-processor firmware image: Failed to write the internal memory.");
    } else if(!(success = device.endTransaction())) {
        setErrorString("Can't flash co-processor firmware image: Failed to end transaction.");
    } else {}

    file->close();

    return success;
}

bool Recovery::upgradeWirelessStack()
{
    debug_msg("Sending FW_UPGRADE command...");

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwUpgrade();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(!success) {
        setErrorString("Can't upgrade Co-Processor firmware: Failed to initiate installation.");
    } else {
        m_deviceState->setStatusString("Upgrading Co-Processor firmware, please wait...");
    }

    return success;
}

bool Recovery::checkWirelessStack()
{
    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    if(!device.beginTransaction()) {
        setErrorString(QStringLiteral("Failed to read co-processor firmware version info"));
        return false;
    }

    const auto versionInfo = device.versionInfo();

    if(!device.endTransaction()) {
        setErrorString(QStringLiteral("Failed to read co-processor firmware version info"));
        return false;
    }

    qCDebug(CATEGORY_DEBUG).noquote() << "FUS version:" << versionInfo.FUSVersion;
    qCDebug(CATEGORY_DEBUG).noquote() << "Wireless Stack version:" << versionInfo.WirelessVersion;

    return versionInfo.WirelessVersion != QStringLiteral("0.0.0");
}

bool Recovery::downloadOptionBytes(QIODevice *file)
{
    m_deviceState->setStatusString("Downloading Option Bytes...");

    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open file for reading");
    const OptionBytes loaded(file);
    file->close();

    check_return_bool(loaded.isValid(), "Failed to load option bytes from file");

    STM32WB55 device(m_deviceState->deviceInfo().usbInfo);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    const OptionBytes actual = device.optionBytes();

    const auto diff = actual.compare(loaded);

    bool success = false;

    if(diff.isEmpty()) {
        debug_msg("Option Bytes OK");

        success = device.leave();

        if(!success) {
            setErrorString("Can't set boot mode: Failed to leave the Recovery mode.");
        }

    } else {
        for(auto it = diff.constKeyValueBegin(); it != diff.constKeyValueEnd(); ++it) {
            debug_msg(QString("Option Bytes mismatch @%1: this: 0x%2, other: 0x%3")
                     .arg((*it).first, to_hex_str(actual.value((*it).first)), to_hex_str((*it).second)));
        }

        debug_msg("Writing corrected Option Bytes...");

        success = device.setOptionBytes(actual.corrected(diff));

        if(!success) {
            setErrorString("Can't set boot mode: Failed to set option bytes");
        }
    }

    begin_ignore_block();
    device.endTransaction();
    end_ignore_block();

    if(success) {
        m_deviceState->setStatusString(QStringLiteral("Exiting recovery mode..."));
    }

    return success;
}
