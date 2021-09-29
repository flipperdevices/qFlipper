#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class RemoveOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    RemoveOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);
    const QString description() const override;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;

    bool parseReceivedData() override;

    QByteArray m_fileName;
};

}
}

