#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class GuiScreenFrameOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    GuiScreenFrameOperation(QSerialPort *serialPort, const QByteArray &screenData, QObject *parent = nullptr);
    const QString description() const override;

private:
    bool begin() override;
    QByteArray m_screenData;
};

}
}

