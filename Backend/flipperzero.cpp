#include "flipperzero.h"

#include <QDebug>
#include <QBuffer>
#include <QIODevice>
#include <QSerialPort>
#include <QtConcurrent/QtConcurrentRun>

#include "serialhelper.h"
#include "dfusedevice.h"
#include "dfusefile.h"
#include "macros.h"

static const auto STARTUP_MESSAGE = QObject::tr("Probing");
static const auto UPDATE_MESSAGE = QObject::tr("Update");
static const auto ERROR_MESSAGE = QObject::tr("Error");

using namespace Flipper;

Zero::Zero(const USBDeviceInfo &parameters, QObject *parent):
    QObject(parent),

    m_info(parameters),

    m_name("N/A"),
    m_target("N/A"),
    m_version("N/A"),
    m_statusMessage(STARTUP_MESSAGE),
    m_progress(0)
{
    if(isDFU()) {
        QtConcurrent::run(this, &Flipper::Zero::fetchInfoDFUMode);
    } else {
        QtConcurrent::run(this, &Flipper::Zero::fetchInfoVCPMode);
    }
}

bool Zero::detach()
{
    const auto portInfo = SerialHelper::findSerialPort(m_info.serialNumber());
    check_return_bool(!portInfo.isNull(), "Could not find serial port");

    QSerialPort port(portInfo);

    check_return_bool(port.open(QIODevice::WriteOnly), "Failed to open serial port");
    check_return_bool((port.write("dfu\r") >= 0) && port.flush(), "Failed to write to serial port");

    port.close();
    return true;
}

bool Zero::download(QIODevice *file)
{
    check_return_bool(file->open(QIODevice::ReadOnly), "Failed to open firmware file");
    check_return_bool(file->bytesAvailable(), "This %^@*$ empty! YEET!");

    setStatusMessage(tr("Updating"));

    DfuseFile fw(file);
    DfuseDevice dev(m_info);

    connect(&dev, &DfuseDevice::progressChanged, this, [=](int operation, double progress) {
        setProgress(progress / 2.0 + (operation == DfuseDevice::Download ? 50 : 0));
    });

    const auto success = dev.beginTransaction() && dev.download(&fw) &&
                         dev.leave() && dev.endTransaction();

    check_continue(success, "Failed to download the firmware");

    file->close();

    return success;
}

const QString &Zero::name() const
{
    return m_name;
}

const QString &Zero::model() const
{
    static const QString m = "Flipper Zero";
    return m;
}

const QString &Zero::target() const
{
    return m_target;
}

const QString &Zero::version() const
{
    return m_version;
}

const QString &Zero::statusMessage() const
{
    return m_statusMessage;
}

double Zero::progress() const
{
    return m_progress;
}

const USBDeviceInfo &Zero::info() const
{
    return m_info;
}

bool Zero::isDFU() const
{
    return m_info.productID() == 0xdf11;
}

void Zero::setName(const QString &name)
{
    if(m_name != name) {
        emit nameChanged(m_name = name);
    }
}

void Zero::setTarget(const QString &target)
{
    if(m_target != target) {
        emit targetChanged(m_target = target);
    }
}

void Zero::setVersion(const QString &version)
{
    if(m_version != version) {
        emit versionChanged(m_version = version);
    }
}

void Zero::setStatusMessage(const QString &message)
{
    if(m_statusMessage != message) {
        emit statusMessageChanged(m_statusMessage = message);
    }
}

void Zero::setProgress(double progress)
{
    if(m_progress != progress) {
        emit progressChanged(m_progress = progress);
    }
}

void Zero::fetchInfoVCPMode()
{
    const auto portInfo = SerialHelper::findSerialPort(m_info.serialNumber());

    if(portInfo.isNull()) {
        // TODO: Error handling
        setStatusMessage(ERROR_MESSAGE);
        error_msg("Port not found");
        return;
    }

    QSerialPort port(portInfo);
    if(!port.open(QIODevice::ReadWrite)) {
        // TODO: Error handling
        setStatusMessage(ERROR_MESSAGE);
        error_msg("Failed to open port");
        return;
    }

    static const auto getValue = [](const QByteArray &buf, const QByteArray &tok) {
        const auto start = buf.indexOf(tok) + tok.size();
        const auto end = buf.indexOf('\n', start);
        return buf.mid(start, end - start).trimmed();
    };

    port.write("hw_info\r");
    port.flush();

    ssize_t bytesAvailable;
    QByteArray buf;

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    buf = port.readAll();

    setName(getValue(buf, "Name: "));
    setTarget(getValue(buf, "HW version:").mid(2, 2).toLower());

    port.write("version\r");
    port.flush();

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    buf = port.readAll();

    setVersion(getValue(buf, "Version:"));

    port.close();
    setStatusMessage(UPDATE_MESSAGE);
}

void Zero::fetchInfoDFUMode()
{
    // TODO: Error handling
    const uint32_t FLIPPER_OTP_ADDRESS = 0x1fff7000UL;
    const size_t FLIPPER_OTP_SIZE = 16, FLIPPER_TARGET_OFFSET = 1, FLIPPER_NAME_OFFSET = 8;
    const int FLIPPER_OTP_ALT_NUM = 2;

    QByteArray otpData;
    QBuffer otpDataBuf(&otpData);
    DfuseDevice dev(m_info);

    otpDataBuf.open(QIODevice::WriteOnly);

    dev.beginTransaction();
    dev.upload(&otpDataBuf, FLIPPER_OTP_ADDRESS, FLIPPER_OTP_SIZE, FLIPPER_OTP_ALT_NUM);
    dev.endTransaction();

    otpDataBuf.close();

    if(!otpData.isEmpty()) {
        setName(otpData.right(FLIPPER_NAME_OFFSET));
        setTarget(QString("f%1").arg((uint8_t)otpData.at(FLIPPER_TARGET_OFFSET)));
    }

    if(m_statusMessage == STARTUP_MESSAGE) {
        setStatusMessage(UPDATE_MESSAGE);
    }
}
