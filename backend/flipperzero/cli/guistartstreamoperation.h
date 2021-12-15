#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiStartStreamOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiStartStreamOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
};

}
}

