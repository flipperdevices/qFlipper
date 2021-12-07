#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class StartStreamOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StartStreamOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
};

}
}

