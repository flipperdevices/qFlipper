#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class MkDirOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:

    MkDirOperation(QSerialPort *serialPort, const QByteArray &dirName, QObject *parent = nullptr);
    const QString description() const override;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
    bool parseReceivedData() override;

    QByteArray m_dirName;
};

}
}

