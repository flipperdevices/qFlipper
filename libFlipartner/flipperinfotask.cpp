#include "flipperinfotask.h"

#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

#include "dfusedevice.h"
#include "serialhelper.h"

FlipperInfoTask::FlipperInfoTask(const FlipperInfo &info):
    m_info(info)
{}

void FlipperInfoTask::run()
{
    if(m_info.isDFU()) {
        getInfoDFUMode();
    } else {
        getInfoNormalMode();
    }

    emit finished(m_info);
}

void FlipperInfoTask::getInfoNormalMode()
{
    const auto portInfo = SerialHelper::findSerialPort(m_info);

    if(portInfo.isNull()) {
        // TODO: Error handling
        return;
    }

    QSerialPort port(portInfo);
    if(!port.open(QIODevice::ReadWrite)) {
        // TODO: Error handling
        return;
    }

    // TODO: Hacks for CLI mode. A proper machine-readable protocol is needed!
    port.write("hw_info\r");
    port.flush();

    ssize_t bytesAvailable;

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    parseHWInfo(port.readAll());

    port.write("version\r");
    port.flush();

    do {
        bytesAvailable = port.bytesAvailable();
        port.waitForReadyRead(50);
    } while(bytesAvailable != port.bytesAvailable());

    parseVersion(port.readAll());
    port.close();
}

void FlipperInfoTask::getInfoDFUMode()
{
    // TODO: Error handling
    const uint32_t FLIPPER_OTP_ADDRESS = 0x1fff7000UL;
    const size_t FLIPPER_OTP_SIZE = 16, FLIPPER_TARGET_OFFSET = 1, FLIPPER_NAME_OFFSET = 8;
    const int FLIPPER_OTP_ALT_NUM = 2;

    QByteArray otpData;
    QBuffer otpDataBuf(&otpData);
    DfuseDevice dev(m_info.params);

    otpDataBuf.open(QIODevice::WriteOnly);

    dev.beginTransaction();
    dev.upload(&otpDataBuf, FLIPPER_OTP_ADDRESS, FLIPPER_OTP_SIZE, FLIPPER_OTP_ALT_NUM);
    dev.endTransaction();

    otpDataBuf.close();
    m_info.target = QString("f%1").arg((uint8_t)otpData.at(FLIPPER_TARGET_OFFSET));
    m_info.name = otpData.right(FLIPPER_NAME_OFFSET);
}

void FlipperInfoTask::parseHWInfo(const QByteArray &buf)
{
    m_info.name = getValue(buf, "Name:");
    m_info.target = getValue(buf, "HW version:").mid(2, 2).toLower();
}

void FlipperInfoTask::parseVersion(const QByteArray &buf)
{
    m_info.version = getValue(buf, "Version:");
}

QByteArray FlipperInfoTask::getValue(const QByteArray &buf, const QByteArray &token)
{
    const auto start = buf.indexOf(token) + token.size();
    const auto end = buf.indexOf('\n', start);
    return buf.mid(start, end - start).trimmed();
}
