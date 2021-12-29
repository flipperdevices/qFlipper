#pragma once

#include "abstractprotobufoperation.h"

namespace Flipper {
namespace Zero {

class StorageStatOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    enum class Type {
        RegularFile,
        Directory,
        Invalid
    };

    Q_ENUM(Type)

    StorageStatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);
    const QString description() const override;

    const QByteArray &fileName() const;
    bool isPresent() const;
    quint64 size() const;
    Type type() const;

private:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    QByteArray m_fileName;
    bool m_isPresent;
    quint64 m_size;
    Type m_type;
};

}
}

