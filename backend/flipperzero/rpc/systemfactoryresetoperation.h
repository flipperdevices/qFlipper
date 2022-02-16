#pragma once

#include "abstractserialoperation.h"

namespace Flipper {
namespace Zero {

class SystemFactoryResetOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    SystemFactoryResetOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onTotalBytesWrittenChanged() override;

private:
    bool begin() override;
    qint64 m_byteCount;
};

}
}

