#include "deviceinfooperation.h"

#include <QBuffer>
#include <QSerialPort>

using namespace Flipper;
using namespace Zero;

DeviceInfoOperation::DeviceInfoOperation(QSerialPort *serialPort, QObject *parent):
    SimpleSerialOperation(serialPort, parent)
{}

const QString DeviceInfoOperation::description() const
{
    return QStringLiteral("Device Info @%1").arg(QString(serialPort()->portName()));
}

const DeviceInfo &DeviceInfoOperation::result() const
{
    return m_deviceInfo;
}

QByteArray DeviceInfoOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n\r\n>: \a");
}

QByteArray DeviceInfoOperation::commandLine() const
{
    return QByteArrayLiteral("device_info\r\n");
}

bool DeviceInfoOperation::parseReceivedData()
{
    QBuffer buf;

    if(!buf.open(QIODevice::ReadWrite)) {
        finishWithError(buf.errorString());
        return false;
    }

    buf.write(receivedData());
    buf.seek(0);

    m_deviceInfo.fusVersion = QStringLiteral("0.0.0");
    m_deviceInfo.radioVersion = QStringLiteral("0.0.0");

    while(buf.canReadLine()) {
        parseLine(buf.readLine());
    }

    buf.close();
    return true;
}

void DeviceInfoOperation::parseLine(const QByteArray &line)
{
    // TODO: Add more fields
    if(line.count(':') != 1) {
        return;
    }

    const auto validx = line.indexOf(':');
    const auto key = line.left(validx).trimmed();
    const auto value = line.mid(validx + 1).trimmed();

    if(key == QByteArrayLiteral("hardware_name")) {
        m_deviceInfo.name = value;
    } else if(key == QByteArrayLiteral("hardware_target")) {
        m_deviceInfo.hardware.target = QStringLiteral("f") + value;
    } else if(key == QByteArrayLiteral("hardware_ver")) {
        m_deviceInfo.hardware.version = value;
    } else if(key == QByteArrayLiteral("hardware_body")) {
        m_deviceInfo.hardware.body = QStringLiteral("b") + value;
    } else if(key == QByteArrayLiteral("hardware_connect")) {
        m_deviceInfo.hardware.connect = QStringLiteral("c") + value;
    } else if(key == QByteArrayLiteral("hardware_color")) {
        m_deviceInfo.hardware.color = (HardwareInfo::Color)value.toInt();
    } else if(key == QByteArrayLiteral("firmware_version")) {
        m_deviceInfo.firmware.version = value;
    } else if(key == QByteArrayLiteral("firmware_commit")) {
        m_deviceInfo.firmware.commit = value;
    } else if(key == QByteArrayLiteral("firmware_branch")) {
        m_deviceInfo.firmware.branch = value;
    } else if(key == QByteArrayLiteral("firmware_build_date")) {
        m_deviceInfo.firmware.date = QDate::fromString(value, QStringLiteral("dd-MM-yyyy"));

    } else if(key == QByteArrayLiteral("radio_stack_major")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(0, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_stack_minor")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(1, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_stack_sub")) {
        auto fields = m_deviceInfo.radioVersion.split('.');
        fields.replace(2, value);
        m_deviceInfo.radioVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_major")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(0, value);
        m_deviceInfo.fusVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_minor")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(1, value);
        m_deviceInfo.fusVersion = fields.join('.');

    } else if(key == QByteArrayLiteral("radio_fus_sub")) {
        auto fields = m_deviceInfo.fusVersion.split('.');
        fields.replace(2, value);
        m_deviceInfo.fusVersion = fields.join('.');
    } else {}
}
