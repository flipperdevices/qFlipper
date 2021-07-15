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
    statusFeedback("Rebooting device in DFU mode...");

    QSerialPort port(SerialHelper::findSerialPort(m_info.serialNumber()));
    const auto success = port.open(QIODevice::WriteOnly) && port.setDataTerminalReady(true) &&
                        (port.write("\rdfu\r") >= 0) && port.flush();
    port.close();

    if(!success) {
        errorFeedback("Failed to reset device to DFU mode");
    }

    return success && waitForReboot();
}

bool FlipperZero::setBootMode(BootMode mode)
{
    info_msg(QString("Setting device to %1 boot mode...").arg(mode == BootMode::Normal ? "NORMAL" : "DFU ONLY"));

    QMutexLocker locker(&m_deviceMutex);

    STM32WB55 device(m_info);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");
    auto ob = device.optionBytes();

    check_return_bool(ob.isValid(), "Failed to read option bytes");

    ob.setNBoot0(mode == BootMode::Normal);
    ob.setNSwBoot0(mode == BootMode::Normal);

    const auto success = device.setOptionBytes(ob) && device.endTransaction();
    check_return_bool(success, "Failed to set option bytes");

    locker.unlock();

    return waitForReboot();
}

bool FlipperZero::waitForReboot(int timeoutMs)
{
    //TODO: Implement better syncronisation
    statusFeedback("Rebooting the device...");

    auto now = QTime::currentTime();
    while(m_isConnected && (now.msecsTo(QTime::currentTime()) < timeoutMs)) {
        QThread::msleep(100);
    }

    if(m_isConnected) {
        errorFeedback("Reboot timeout exceeded.");
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
        errorFeedback("Reconnect timeout exceeded.");
    }

    return m_isConnected;
}

bool FlipperZero::isFUSRunning()
{
    info_msg("Checking if FUS is RUNNING...");

    QMutexLocker locker(&m_deviceMutex);

    STM32WB55 device(m_info);
    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");

    auto state = device.FUSGetState();
    const auto running = (state.status() == FUSState::Idle) &&
                         (state.error() == FUSState::NoError);
    check_return_bool(device.endTransaction(), "Failed to end transaction");
    info_msg(running ? "FUS is RUNNING" : "FUS is NOT running");

    return running;
}

bool FlipperZero::startFUS()
{
    info_msg("Attempting to start FUS...");

    QMutexLocker locker(&m_deviceMutex);

    STM32WB55 device(m_info);
    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");

    auto state = device.FUSGetState();
    const auto running = (state.status() == FUSState::Idle) &&
                         (state.error() == FUSState::NoError);
    if(running) {
        info_msg("FUS is already RUNNING, doing nothing...");
        check_return_bool(device.endTransaction(), "Failed to end transaction");
        return true;
    }

    info_msg(QString("FUS appears not to be running: %1, %2.").arg(state.statusString(), state.errorString()));

    // Send a second GET_STATE to actually start FUS
    begin_ignore_block();
    state = device.FUSGetState();
    end_ignore_block();

    check_return_bool(device.endTransaction(), "Failed to end transaction");

    // At this point, there is no way to know whether FUS has actually started, but things are looking as expected.
    locker.unlock();
    return waitForReboot();
}

bool FlipperZero::startWirelessStack()
{
    QMutexLocker locker(&m_deviceMutex);

    STM32WB55 device(m_info);
    const auto success = device.beginTransaction() && device.FUSStartWirelessStack();
    check_return_bool(success, "Failed to start wireless stack");

    locker.unlock();

    return waitForReboot();
}

bool FlipperZero::eraseWirelessStack()
{
    info_msg("Attempting to erase WIRELESS STACK...");

    QMutexLocker locker(&m_deviceMutex);

    STM32WB55 device(m_info);
    const auto success = device.beginTransaction() && device.FUSFwDelete() && device.endTransaction();
    check_return_bool(success, "Failed to initiate wireless stack erase");

    locker.unlock();

    // Reboot loop - handles multiple device reboots
    for(;;) {
        QMutexLocker locker(&m_deviceMutex);
        STM32WB55 device(m_info);

        // Status loop - Polls device status for completion
        for(;;) {
            if(!device.beginTransaction()) {
                info_msg("Device seems to have REBOOTED itself, waiting... 1");
                break;
            }

            const auto state = device.FUSGetState();
            if(!state.isValid()) {
                info_msg("Device seems to have REBOOTED itself, waiting... 2");
                break;
            }

            info_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));

            const auto done = (state.status() == FUSState::Idle) && (state.error() == FUSState::NoError);

            if(done) {
                check_return_bool(device.endTransaction(), "Failed to end transaction");
                info_msg("Wireless stack erase COMPLETE.");
                return true;
            }

            QThread::msleep(1000);
        }

        locker.unlock();

        if(!waitForReboot()) {
            error_msg("Failed to erase wireless stack: Reboot TIMEOUT");
            break;
        }
    }

    return false;
}

bool FlipperZero::downloadFirmware(QIODevice *file)
{

    QMutexLocker locker(&m_deviceMutex);

    if(!file->open(QIODevice::ReadOnly)) {
        errorFeedback("Failed to open the file.");
        return false;

    } else if(file->bytesAvailable() <= 0) {
        errorFeedback("The file is empty.");
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

    if(!success) {
        errorFeedback("Failed to download the firmware.");
    }

    return success && waitForReboot();
}

bool FlipperZero::downloadWirelessStack(QIODevice *file, uint32_t addr)
{
    info_msg("Downloading WIRELESS STACK...");

    QMutexLocker locker(&m_deviceMutex);

    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open firmware file");
    check_return_bool(file->bytesAvailable(), "The firmware file is empty");

    STM32WB55 device(m_info);

    check_return_bool(device.beginTransaction(), "Failed to initiate transaction");

    if(!addr) {
        const auto ob = device.optionBytes();
        check_return_bool(ob.isValid(), "Failed to get option bytes");

        const auto origin = device.partitionOrigin((uint8_t)STM32WB55::Partition::Flash);
        const auto pageSize = (uint32_t)0x1000; // TODO: do not hardcode page size
        addr = (origin + (pageSize * ob.SFSA()) - file->bytesAvailable()) & (~(pageSize - 1));

        info_msg(QString("SFSA value is 0x%1").arg(QString::number(ob.SFSA(), 16)));
        info_msg(QString("Target address for wireless stack is 0x%1").arg(QString::number(addr, 16)));

    } else {
        info_msg(QString("Target address for wireless stack image is OVERRIDDEN to 0x%1").arg(QString::number(addr, 16)));
    }

    connect(&device, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = device.erase(addr, file->bytesAvailable()) &&
                         device.download(file, addr, 0) && device.endTransaction();
    file->close();

    check_return_bool(success, "Failed to download wireless stack image");
    return true;
}

bool FlipperZero::upgradeWirelessStack()
{
    info_msg("Sending FW_UPGRADE command...");

    QMutexLocker locker(&m_deviceMutex);
    STM32WB55 device(m_info);

    const auto success = device.beginTransaction() && device.FUSFwUpgrade() && device.endTransaction();
    check_return_bool(success, "Failed to send FW_UPGRADE command");

    locker.unlock();

    info_msg("WAITING for FW_UPGRADE to finish...");

    // Reboot loop - handles multiple device reboots
    for(;;) {
        QMutexLocker locker(&m_deviceMutex);
        STM32WB55 device(m_info);

        // Status loop - Polls device status for completion
        for(;;) {
            if(!device.beginTransaction()) {
                info_msg("Device seems to have REBOOTED itself, waiting... 1");
                break;
            }

            const auto state = device.FUSGetState();
            if(!state.isValid()) {
                info_msg("Device seems to have REBOOTED itself, waiting... 2");
                break;
            }

            info_msg(QString("Current FUS state: %1, %2").arg(state.statusString(), state.errorString()));

            const auto done = state.error() == FUSState::NotRunning ||
                              state.status() == FUSState::Idle;
            if(done) {
                check_return_bool(device.endTransaction(), "Failed to end transaction");
                info_msg("Firmware upgrade COMPLETE.");
                return true;
            }

            QThread::msleep(1000);
        }

        locker.unlock();

        if(!waitForReboot()) {
            error_msg("Failed to upgrade device firmware: Reboot TIMEOUT");
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
        const auto msg = "Failed to open serial port.";
        error_msg(msg + port.errorString());
        setError(tr(msg));
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
        const auto msg = "Failed to read from serial port.";
        error_msg(msg);
        setError(tr(msg)); // TODO: Ask user to check if other apps are using the port (e.g. CLI)
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
            setError(tr(msg));
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
}

}
