#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiStartVirtualDisplayOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiStartVirtualDisplayOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
    QByteArray m_screenData;
};

}
}

