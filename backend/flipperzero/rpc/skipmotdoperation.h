#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class SkipMOTDOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    SkipMOTDOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private:
    QByteArray endOfMessageToken() const override;
    uint32_t flags() const override;
};

}
}

