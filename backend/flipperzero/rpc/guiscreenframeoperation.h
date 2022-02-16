#pragma once

#include "abstractserialoperation.h"

namespace Flipper {
namespace Zero {

class GuiScreenFrameOperation : public AbstractSerialOperation
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

