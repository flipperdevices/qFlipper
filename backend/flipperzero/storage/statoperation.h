#pragma once

#include "storageoperation.h"

namespace Flipper {
namespace Zero {

class StatOperation : public StorageOperation
{
    Q_OBJECT

    enum State {
        SkippingReply = BasicState::User,
        ReadingReply
    };

public:
    enum class Type{
        File,
        Directory,
        Storage,
        NotFound,
        Invalid
    };

    StatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);

    const QString description() const override;

    const QByteArray &fileName() const;
    quint64 size() const;
    Type type() const;

private slots:
    void onSerialPortReadyRead() override;
    void onOperationTimeout() override;

private:
    bool begin() override;
    bool parseReceivedData();

    QByteArray m_fileName;
    QByteArray m_receivedData;
    quint64 m_size;
    Type m_type;
};

}
}

