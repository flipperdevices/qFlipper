#include "flipperzero.h"

#include <QTime>
#include <QBuffer>
#include <QIODevice>
#include <QSerialPort>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentRun>

#include "remotecontroller.h"
#include "serialhelper.h"
#include "dfusedevice.h"
#include "factoryinfo.h"
#include "dfusefile.h"
#include "macros.h"

#include "device/stm32wb55.h"

#define ARBITRARY_NUMBER 66 // Execute order Sixty-Six!

#define try_run(condition, errorMsg) \
    if(!(condition)) { \
        errorFeedback(errorMsg); \
        return false; \
    }

/* ----------------------------------------------------------------------------------------------------------------------------------
 * FUS operations are based on the info from AN5185
 * https://www.st.com/resource/en/application_note/dm00513965-st-firmware-upgrade-services-for-stm32wb-series-stmicroelectronics.pdf
 * ---------------------------------------------------------------------------------------------------------------------------------- */

namespace Flipper {

using namespace Zero;
using namespace STM32;
using namespace WB55;

FlipperZero::FlipperZero(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),

    m_isPersistent(false),
    m_isConnected(true),
    m_isError(false),

    m_name("N/A"),
    m_target("N/A"),
    m_version("N/A"),
    m_progress(0),

    m_remote(nullptr)
{
    setDeviceInfo(info);
}

void FlipperZero::setDeviceInfo(const USBDeviceInfo &info)
{
    m_info = info;

    if(isDFU()) {
        if(m_remote) {
            m_remote->deleteLater();
            m_remote = nullptr;
        }

        fetchInfoDFUMode();

    } else {
        const auto portInfo = SerialHelper::findSerialPort(info.serialNumber());

        if(portInfo.isNull()) {
            const auto msg = "Can't find serial port";
            error_msg(msg);
            setError(tr(msg));

            return;
        }

        if(m_remote) {
            m_remote->deleteLater();
        }

        m_remote = new Zero::RemoteController(portInfo, this);

        fetchInfoVCPMode();
    }

    emit isDFUChanged();

    setProgress(0);
    setConnected(true);
    setError(QString(), false);
}

void FlipperZero::setPersistent(bool set)
{
    if(set == m_isPersistent) {
        return;
    }

    m_isPersistent = set;
    emit isPersistentChanged();
}

void FlipperZero::setConnected(bool set)
{
    if(set == m_isConnected) {
        return;
    }

    m_isConnected = set;
    emit isConnectedChanged();
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

bool FlipperZero::isConnected() const
{
    return m_isConnected;
}

bool FlipperZero::isError() const
{
    return m_isError;
}

bool FlipperZero::detach()
{
    statusFeedback("Switching device to DFU mode...");

    QSerialPort port(SerialHelper::findSerialPort(m_info.serialNumber()));
    const auto success = port.open(QIODevice::WriteOnly) && port.setDataTerminalReady(true) &&
                        (port.write("\rdfu\r") >= 0) && port.flush();
    port.close();

    if(!success) {
        errorFeedback("Can't detach the device: Failed to reset in DFU mode");
        return false;
    }

    return waitForReboot();
}

bool FlipperZero::setBootMode(BootMode mode)
{
    info_msg(QString("Setting device to %1 boot mode...").arg(mode == BootMode::Normal ? "NORMAL" : "DFU ONLY"));

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    if(!device.beginTransaction()) {
        errorFeedback("Can't set boot mode: Failed to initiate transaction.");
        return false;
    }

    auto ob = device.optionBytes();

    if(!ob.isValid()) {
        errorFeedback("Can't set boot mode: Failed to read option bytes.");
        return false;
    }

    ob.setNBoot0(mode == BootMode::Normal);
    ob.setNSwBoot0(mode == BootMode::Normal);

    if(!device.setOptionBytes(ob)) {
        errorFeedback("Cant' set boot mode: Failed to set option bytes");
        return false;
    }

    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    locker.unlock();

    return waitForReboot();
}

bool FlipperZero::waitForReboot(int timeoutMs)
{
    //TODO: Implement better syncronisation
    auto now = QTime::currentTime();
    while(m_isConnected && (now.msecsTo(QTime::currentTime()) < timeoutMs)) {
        QThread::msleep(100);
    }

    if(m_isConnected) {
        errorFeedback("Failed to reboot the device: Reboot timeout exceeded.");
        return false;
    }

    info_msg("Device has successfully DISCONNECTED, waiting for it to reconnect...");

    now = QTime::currentTime();
    while(!m_isConnected && (now.msecsTo(QTime::currentTime()) < timeoutMs)) {
        QThread::msleep(100);
    }

    if(m_isConnected) {
        info_msg("Device has SUCCESSFULLY rebooted.")
    } else {
        errorFeedback("Failed to reboot the device: Reconnect timeout exceeded.");
    }

    return m_isConnected;
}

bool FlipperZero::isFUSRunning()
{
    info_msg("Checking whether FUS is RUNNING...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    if(!device.beginTransaction()) {
        errorFeedback("Can't check FUS status: Failed to initiate transaction.");
        return false;
    }

    auto state = device.FUSGetState();
    if(!state.isValid()) {
        errorFeedback("Can't check FUS status: Failed to get FUS status.");
        return false;
    }

    const auto running = (state.status() == FUSState::Idle) &&
                         (state.error() == FUSState::NoError);

    if(!device.endTransaction()) {
        errorFeedback("Can't check FUS status: Failed to end transaction.");
        return false;
    }

    if(running) {
        statusFeedback("FUS has been successfully started.");
    } else {
        info_msg(QString("FUS is NOT running: %1, %2").arg(state.statusString(), state.errorString()));
    }

    return running;
}

bool FlipperZero::startFUS()
{
//    info_msg("Attempting to start FUS...");
    statusFeedback("Starting firmware upgrade service (FUS)...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    if(!device.beginTransaction()) {
        errorFeedback("Can't start FUS: Failed to initiate transaction.");
        return false;
    }

    auto state = device.FUSGetState();

    if(!state.isValid()) {
        errorFeedback("Can't start FUS: Failed to get FUS status.");
        return false;
    }

    const auto running = (state.status() == FUSState::Idle) && (state.error() == FUSState::NoError);

    if(running) {
        info_msg("FUS is already RUNNING, doing nothing...");

        if(!device.endTransaction()) {
            errorFeedback("Can't start FUS: Failed to end transaction.");
            return false;
        }

        return true;
    }

    info_msg(QString("FUS appears NOT to be running: %1, %2.").arg(state.statusString(), state.errorString()));

    // Send a second GET_STATE to actually start FUS
    begin_ignore_block();
    state = device.FUSGetState();
    end_ignore_block();

    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");
    // At this point, there is no way to know whether FUS has actually started, but things are looking as expected.
    locker.unlock();
    return waitForReboot();
}

// TODO: check status to see if the wireless stack is present at all
bool FlipperZero::startWirelessStack()
{
    statusFeedback("Attempting to start the wireless stack...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    auto success = device.beginTransaction() && device.FUSStartWirelessStack();// &&device.endTransaction();

    if(!success) {
        errorFeedback("Failed to start wireless stack");
        return false;
    }

    const auto state = device.FUSGetState();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(state.isValid()) {
        info_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));
        return true;
    } else {
        locker.unlock();
        return waitForReboot();
    }

    return false;
}

bool FlipperZero::deleteWirelessStack()
{
    info_msg("Attempting to delete CO-PROCESSOR firmware...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    const auto success = device.beginTransaction() && device.FUSFwDelete() && device.endTransaction();

    if(!success) {
        errorFeedback("Can't install co-processor firmware: Failed to initiate firmware removal.");
        return false;
    }

    locker.unlock();

    // Reboot loop - handles multiple device reboots
    for(;;) {
        STM32WB55 device(m_info);

        // Status loop - Polls device status for completion
        for(;;) {
            QMutexLocker locker(&m_deviceMutex);

            if(!isConnected() || !device.beginTransaction()) {
                info_msg("Device seems to have REBOOTED itself BEFORE getting state, waiting...");
                break;
            }

            const auto state = device.FUSGetState();
            if(!state.isValid()) {
                info_msg("Device seems to have REBOOTED itself, waiting... 2");
                break;
            }

            check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

            if(state.status() == FUSState::Idle) {
                statusFeedback("Deleted the old co-processor firmware.");
                return true;

            } else if(state.status() == FUSState::ErrorOccured) {
                errorFeedback("Can't delete co-processor firmware: An error has occured during the operation.");
                info_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));
                return false;

            } else {}

            QThread::msleep(1000);
        }

        if(!waitForReboot()) {
            errorFeedback("Can't delete co-processor firmware: Device reboot timeout.");
            break;
        }
    }

    return false;
}

bool FlipperZero::downloadFirmware(QIODevice *file)
{

    QMutexLocker locker(&m_deviceMutex);

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
    DfuseDevice dev(m_info);

    file->close();

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) &&
                         dev.leave() && dev.endTransaction();
    locker.unlock();

    if(success) {
        statusFeedback("Booting the device up...");
    } else {
        errorFeedback("Can't download firmware: An error has occured during the operation.");
    }

    return success && waitForReboot();
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

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

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

        addr = (origin + (pageSize * ob.SFSA()) - file->bytesAvailable()) & (~(pageSize - 1));

        info_msg(QString("SFSA value is 0x%1").arg(QString::number(ob.SFSA(), 16)));
        info_msg(QString("Target address for wireless stack is 0x%1").arg(QString::number(addr, 16)));

    } else {
        info_msg(QString("Target address for wireless stack image has been OVERRIDDEN to 0x%1").arg(QString::number(addr, 16)));
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

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    const auto success = device.beginTransaction() && device.FUSFwUpgrade() && device.endTransaction();

    if(!success) {
        errorFeedback("Can't install co-processor firmware: Failed to initiate installation.");
        return false;
    }

    locker.unlock();

    info_msg("WAITING for FW_UPGRADE to finish...");

    // Reboot loop - handles multiple device reboots
    for(;;) {
        STM32WB55 device(m_info);

        // Status loop - Polls device status for completion
        for(;;) {
            QMutexLocker locker(&m_deviceMutex);

            if(!isConnected() || !device.beginTransaction()) {
                info_msg("Device seems to have REBOOTED itself BEFORE getting state, waiting...");
                break;
            }

            const auto state = device.FUSGetState();
            if(!state.isValid()) {
                info_msg("Device seems to have REBOOTED itself WHILE getting state, waiting...");
                break;
            }

            check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

            if(state.status() == FUSState::ErrorOccured) {
                const auto done = (state.error() == FUSState::NotRunning);
                if(done) {
                    statusFeedback("Wireless stack installation complete.");
                } else {
                    errorFeedback("Failed to install co-processor firmware.");
                    error_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));
                }

                return done;

            } else if(state.status() == FUSState::Idle) {
                statusFeedback("FUS installation complete.");
                return true;

            } else if(state.status() == FUSState::FWUpgradeOngoing) {
                statusFeedback("Installing Wireless stack update, please wait...");
            } else if(state.status() == FUSState::FUSUpgradeOngoing) {
                statusFeedback("Installing FUS update, please wait...");
            } else {}

            QThread::msleep(1000);
        }

        if(!waitForReboot()) {
            errorFeedback("Can't install co-processor firmware: Device reboot timeout.");
            break;
        }
    }

    return false;
}

const QString &FlipperZero::name() const
{
    return m_name;
}

const QString &FlipperZero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &FlipperZero::target() const
{
    return m_target;
}

const QString &FlipperZero::version() const
{
    return m_version;
}

const QString &FlipperZero::statusMessage() const
{
    return m_statusMessage;
}

double FlipperZero::progress() const
{
    return m_progress;
}

const USBDeviceInfo &FlipperZero::info() const
{
    return m_info;
}

bool FlipperZero::isDFU() const
{
    return m_info.productID() == 0xdf11;
}

Flipper::Zero::RemoteController *FlipperZero::remote() const
{
    return m_remote;
}

void FlipperZero::setName(const QString &name)
{
    if(m_name != name) {
        emit nameChanged(m_name = name);
    }
}

void FlipperZero::setTarget(const QString &target)
{
    if(m_target != target) {
        emit targetChanged(m_target = target);
    }
}

void FlipperZero::setVersion(const QString &version)
{
    if(m_version != version) {
        emit versionChanged(m_version = version);
    }
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

void FlipperZero::fetchInfoVCPMode()
{
    info_msg("Fetching device info in VCP MODE...");

    QSerialPort port(SerialHelper::findSerialPort(m_info.serialNumber()));

    if(!port.open(QIODevice::ReadWrite)) {
        errorFeedback("Failed to open serial port.\nIs there a CLI session open?");
        return;
    }

    static const auto getValue = [](const QByteArray &line) {
        const auto fields = line.split(':');

        if(fields.size() != 2) {
            return QByteArray();
        }

        return fields.last().trimmed();
    };

    port.setDataTerminalReady(true);
    port.write("\rdevice_info\r");
    port.flush();

    qint64 bytesAvailable;

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    // A hack for Linux systems which seem to allow opening a serial port twice.
    if(bytesAvailable < ARBITRARY_NUMBER) {
        errorFeedback("Failed to read from serial port.\nIs there a CLI session open?");
        return;
    }

    do {
        const auto line = port.readLine();

        if(line.startsWith("hardware_name")) {
            setName(getValue(line));
        } else if(line.startsWith("hardware_target")) {
            setTarget("f" + getValue(line));
        } else if(line.startsWith("firmware_version")) {
            setVersion(getValue(line));
        } else {}

    } while(port.canReadLine());

    port.close();
}

void FlipperZero::fetchInfoDFUMode()
{
    info_msg("Fetching device info in DFU MODE...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    check_return_void(device.beginTransaction(), "Failed to initiate transaction");
    const FactoryInfo factoryInfo(device.OTPData(FactoryInfo::size()));
    check_return_void(device.endTransaction(), "Failed to end transaction");

    if(factoryInfo.isValid()) {
        setTarget(QString("f%1").arg(factoryInfo.target()));
        setName(factoryInfo.name());

    } else {
        const auto msg = "Failed to get device information.";
        error_msg(msg);

        if(!isPersistent()) {
            errorFeedback(msg);
        }
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
    setError(tr("ERROR: ") + tr(msg));
    setPersistent(false);
}

}
