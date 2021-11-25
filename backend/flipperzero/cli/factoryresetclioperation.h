#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class FactoryResetCliOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    FactoryResetCliOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
    bool parseReceivedData() override;
};

}
}

