#pragma once

#include "abstractserialoperation.h"

#include <QByteArray>

class QIODevice;

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
    ReadOperation(QSerialPort *serialPort, const QByteArray &fileName, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    bool parseError();
    bool parseSetupReply();
    bool parseSize(const QByteArray &s);

    qint64 m_size;
    QByteArray m_fileName;
    QIODevice *m_file;
    QByteArray m_receivedData;
};

}
}

