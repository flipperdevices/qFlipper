#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class StatOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    enum class Type{
        File,
        Directory,
        Storage,
        NotFound,
        InternalError,
        Invalid
    };

    StatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);

    const QString description() const override;

    const QByteArray &fileName() const;
    quint64 size() const;
    Type type() const;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
    bool parseReceivedData() override;

    QByteArray m_fileName;
    quint64 m_size;
    Type m_type;
};

}
}

