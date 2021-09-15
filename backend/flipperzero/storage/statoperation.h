#pragma once

#include "simpleserialoperation.h"

namespace Flipper {
namespace Zero {

class StatOperation : public SimpleSerialOperation
{
    Q_OBJECT

public:
    enum class Type {
        File,
        Directory,
        Storage,
        NotFound,
        InternalError,
        Invalid
    };

    Q_ENUM(Type)

    StatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);

    const QString description() const override;

    const QByteArray &fileName() const;
    qint64 size() const;
    qint64 sizeFree() const;
    Type type() const;

private:
    QByteArray endOfMessageToken() const override;
    QByteArray commandLine() const override;
    bool parseReceivedData() override;

    bool parseFileSize(const QByteArray &data);
    bool parseStorageSize(const QByteArray &data);

    QByteArray m_fileName;
    qint64 m_size;
    qint64 m_sizeFree;
    Type m_type;
};

}
}

