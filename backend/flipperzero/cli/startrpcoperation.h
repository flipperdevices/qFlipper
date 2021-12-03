#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class StartRPCOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    StartRPCOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
};

}
}

