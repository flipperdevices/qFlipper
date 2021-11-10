#pragma once

#include "simpleserialoperation.h"
#include "flipperzero/deviceinfo.h"

namespace Flipper {
namespace Zero {

class DeviceInfoOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    DeviceInfoOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;
    const DeviceInfo &result() const;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
    bool parseReceivedData() override;

    void parseLine(const QByteArray &line);

    DeviceInfo m_deviceInfo;
};

}
}

