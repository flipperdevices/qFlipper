#include "flipperzero.h"

#include <QTime>
#include <QBuffer>
#include <QIODevice>
#include <QSerialPort>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrentRun>

#include "deviceinfofetcher.h"
#include "remotecontroller.h"
#include "serialfinder.h"
#include "dfusedevice.h"
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

FlipperZero::FlipperZero(const USBDeviceInfo &info, QObject *parent):
    QObject(parent),

    m_isPersistent(false),
    m_isOnline(false),
    m_isError(false),

    m_usbInfo(info),

    m_progress(0),
    m_serialPort(nullptr),
    m_remote(nullptr)
{
    if(isDFU()) {
        fetchDeviceInfo();

    } else {
        auto *finder = new SerialFinder(info.serialNumber(), this);
        connect(finder, &SerialFinder::finished, this, &FlipperZero::initVCPMode);
        connect(finder, &SerialFinder::finished, finder, &SerialFinder::deleteLater);
    }
}

FlipperZero::~FlipperZero()
{
    setOnline(false);
}

void FlipperZero::reuse(const FlipperZero *other)
{
    setProgress(0);

    setUSBInfo(other->usbInfo());
    setDeviceInfo(other->deviceInfo());

    setSerialPort(other->m_serialPort);
    setRemoteController(other->remote());

    setOnline(true);
}

void FlipperZero::setUSBInfo(const USBDeviceInfo &info)
{
    // Not checking the huge structure for equality
    m_usbInfo = info;
    emit usbInfoChanged();
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

bool FlipperZero::detach()
{
    statusFeedback("Switching device to <b>DFU</b> mode...");

    QSerialPort port(SerialFinder::findSerialPort(m_usbInfo.serialNumber()));
    const auto success = port.open(QIODevice::WriteOnly) && port.setDataTerminalReady(true) &&
                        (port.write("\rdfu\r") >= 0);

    auto flushTries = 100;
    while(--flushTries && !port.flush()) {
        info_msg("Serial port flush failure, retrying...");
        QThread::usleep(1000);
    }

    if(!success || !flushTries) {
        errorFeedback("Can't detach the device: Failed to reset in DFU mode");
        error_msg(QString("Serial port status: %1").arg(port.errorString()));
        return false;
    }

    port.close();

    return waitForReboot();
}

bool FlipperZero::setBootMode(BootMode mode)
{
    const auto msg = (mode == BootMode::Normal) ? "Booting the device up..." : "Setting device to <b>DFU boot</b> mode...";
    statusFeedback(msg);

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

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
    while(m_isOnline && (now.msecsTo(QTime::currentTime()) < timeoutMs)) {
        QThread::usleep(1000);
    }

    if(m_isOnline) {
        errorFeedback("Failed to reboot the device: Reboot timeout exceeded.");
        return false;
    }

    info_msg("Device has successfully DISCONNECTED, waiting for it to reconnect...");

    now = QTime::currentTime();
    while(!m_isOnline && (now.msecsTo(QTime::currentTime()) < timeoutMs)) {
        QThread::usleep(1000);
    }

    if(m_isOnline) {
        info_msg("Device has SUCCESSFULLY rebooted.")
    } else {
        errorFeedback("Failed to reboot the device: Reconnect timeout exceeded.");
    }

    return m_isOnline;
}

bool FlipperZero::isFUSRunning()
{
    info_msg("Checking whether FUS is RUNNING...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

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
    STM32WB55 device(m_usbInfo);

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
    statusFeedback("Attempting to start the Wireless Stack...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

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
    statusFeedback("Deleting old co-processor firmware...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwDelete() && device.endTransaction();

    if(!success) {
        errorFeedback("Can't delete old co-processor firmware: Failed to initiate firmware removal.");
        return false;
    }

    locker.unlock();

    check_continue(waitForReboot(5000), "Sorry for the wait, the device seem to have already rebooted.");

    // Reboot loop - handles multiple device reboots
    for(;;) {
        STM32WB55 device(m_usbInfo);

        // Status loop - Polls device status for completion
        for(;;) {
            QMutexLocker locker(&m_deviceMutex);

            if(!isOnline() || !device.beginTransaction()) {
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
                errorFeedback("Can't delete old co-processor firmware: An error has occured during the operation.");
                info_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));
                return false;

            } else {}

            QThread::msleep(1000);
        }

        if(!waitForReboot()) {
            errorFeedback("Can't delete old co-processor firmware: Device reboot timeout.");
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
    DfuseDevice dev(m_usbInfo);

    file->close();

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) && dev.leave();
    check_continue(dev.endTransaction(), "^^^ It's probably nothing at this point... ^^^");
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
    STM32WB55 device(m_usbInfo);

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

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

    const auto success = device.beginTransaction() && device.FUSFwUpgrade();
    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    if(!success) {
        errorFeedback("Can't install co-processor firmware: Failed to initiate installation.");
        return false;
    }

    locker.unlock();

    info_msg("WAITING for FW_UPGRADE to finish...");

    // Reboot loop - handles multiple device reboots
    for(;;) {
        STM32WB55 device(m_usbInfo);

        // Status loop - Polls device status for completion
        for(;;) {
            QMutexLocker locker(&m_deviceMutex);

            if(!isOnline() || !device.beginTransaction()) {
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

bool FlipperZero::fixOptionBytes(QIODevice *file)
{
    statusFeedback("Fixing Option Bytes...");

    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open file for reading");
    const OptionBytes loaded(file);
    file->close();

    check_return_bool(loaded.isValid(), "Failed to load option bytes from file");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_usbInfo);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    const OptionBytes actual = device.optionBytes();

    const auto diff = actual.compare(loaded);

    if(diff.isEmpty()) {
        info_msg("Option Bytes OK");
        device.leave();

    } else {
        for(auto it = diff.constKeyValueBegin(); it != diff.constKeyValueEnd(); ++it) {
            info_msg(QString("Option Bytes mismatch @%1: this: 0x%2, other: 0x%3")
                     .arg((*it).first, to_hex_str(actual.value((*it).first)), to_hex_str((*it).second)));
        }

        info_msg("Writing corrected Option Bytes");
        device.setOptionBytes(actual.corrected(diff));
    }

    check_continue(device.endTransaction(), "^^^ It's probably nothing at this point... ^^^");

    locker.unlock();
    return waitForReboot();
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
    return m_usbInfo;
}

const DeviceInfo &FlipperZero::deviceInfo() const
{
    return m_deviceInfo;
}

bool FlipperZero::isDFU() const
{
    return m_usbInfo.productID() == 0xdf11;
}

Flipper::Zero::RemoteController *FlipperZero::remote() const
{
    return m_remote;
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

void FlipperZero::initVCPMode(const QSerialPortInfo &portInfo)
{
    if(portInfo.isNull()) {
        error_msg("Failed to find a suitable serial port.");
        return;
    }

    setSerialPort(new QSerialPort(portInfo, this));
    setRemoteController(new RemoteController(m_serialPort, this));

    fetchDeviceInfo();
}

void FlipperZero::fetchDeviceInfo()
{
    AbstractDeviceInfoFetcher *fetcher;

    if(isDFU()) {
        fetcher = new DFUDeviceInfoFetcher(m_usbInfo, this);
    } else {
        fetcher = new VCPDeviceInfoFetcher(m_serialPort, this);
    }

    connect(fetcher, &AbstractDeviceInfoFetcher::finished, this, [=]() {
        if(fetcher->isError()) {
            // TODO: Display the error in UI
            error_msg(QStringLiteral("Failed to fetch device info: %1.").arg(fetcher->errorString()));
            return;
        }

        setDeviceInfo(fetcher->result());
        setOnline(true);
    });

    connect(fetcher, &AbstractDeviceInfoFetcher::finished, fetcher, &QObject::deleteLater);

    fetcher->fetch();
}

void FlipperZero::setSerialPort(QSerialPort *serialPort)
{
    if(m_serialPort) {
        m_serialPort->deleteLater();
    }

    m_serialPort = serialPort;

    if(m_serialPort) {
        m_serialPort->setParent(this);
    }
}

void FlipperZero::setRemoteController(Zero::RemoteController *remote)
{
    if(m_remote) {
        m_remote->deleteLater();
    }

    m_remote = remote;

    if(m_remote) {
        m_remote->setParent(this);
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
