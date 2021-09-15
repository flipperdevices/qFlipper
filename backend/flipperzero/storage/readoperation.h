#pragma once

#include "abstractserialoperation.h"

#include <QByteArray>

namespace Flipper {
namespace Zero {

class ReadOperation : public AbstractSerialOperation
{
    Q_OBJECT

    enum State {
        SettingUp = BasicState::Ready,
        ReceivingData
    };

public:
    ReadOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray &result() const;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    qint64 m_size;
    qint64 m_lineCount;
    QByteArray m_fileName;
    QByteArray m_receivedData;
};

}
}

