#include "flipperzero.h"

#include <QDebug>
#include <QBuffer>
#include <QSerialPort>

#include "informationfetcher.h"

#include "serialhelper.h"
#include "dfusedevice.h"
#include "macros.h"

using namespace Flipper;

Zero::Zero(const USBDeviceParams &parameters, QObject *parent):
    QObject(parent),
    m_parameters(parameters),
    m_name("N/A"), m_target("N/A"), m_version("N/A")
{
    if(!USBDevice::backend().getExtraDeviceInfo(m_parameters)) {
        setName("ERROR");
        return;
    }

    if(isDFU()) {
        fetchInfoDFUMode();
    } else {
        fetchInfoNormalMode();
    }
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

bool Zero::isDFU() const
{
    return m_parameters.productID == 0xdf11;
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

void *Zero::uniqueID() const
{
    return m_parameters.uniqueID;
}

void Zero::fetchInfoNormalMode()
{
    const auto portInfo = SerialHelper::findSerialPort(m_parameters.serialNumber);

    if(portInfo.isNull()) {
        // TODO: Error handling
        error_msg("Port not found");
        return;
    }

    QSerialPort port(portInfo);
    if(!port.open(QIODevice::ReadWrite)) {
        // TODO: Error handling
        error_msg("Failed to open port")
        return;
    }

    static const auto getValue = [](const QByteArray &buf, const QByteArray &tok) -> QByteArray {
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
}

void Zero::fetchInfoDFUMode()
{
    // TODO: Error handling
    const uint32_t FLIPPER_OTP_ADDRESS = 0x1fff7000UL;
    const size_t FLIPPER_OTP_SIZE = 16, FLIPPER_TARGET_OFFSET = 1, FLIPPER_NAME_OFFSET = 8;
    const int FLIPPER_OTP_ALT_NUM = 2;

    QByteArray otpData;
    QBuffer otpDataBuf(&otpData);
    DfuseDevice dev(m_parameters);

    otpDataBuf.open(QIODevice::WriteOnly);

    dev.beginTransaction();
    dev.upload(&otpDataBuf, FLIPPER_OTP_ADDRESS, FLIPPER_OTP_SIZE, FLIPPER_OTP_ALT_NUM);
    dev.endTransaction();

    otpDataBuf.close();

    setName(otpData.right(FLIPPER_NAME_OFFSET));
    setTarget(QString("f%1").arg((uint8_t)otpData.at(FLIPPER_TARGET_OFFSET)));
}
