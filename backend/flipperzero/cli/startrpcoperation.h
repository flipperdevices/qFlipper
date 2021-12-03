#pragma once

#include "abstractserialoperation.h"

namespace Flipper {
namespace Zero {

class StartRPCOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    StartRPCOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
};

}
}

