#include "flipperinfotask.h"

#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

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
    // Not implemented yet
    m_info.name = "DFU Mode";
}

void FlipperInfoTask::parseHWInfo(const QByteArray &buf)
{
    m_info.name = getValue(buf, "Name:");
    m_info.target = getValue(buf, "HW version:").mid(2, 2);
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
