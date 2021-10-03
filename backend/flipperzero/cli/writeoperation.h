#pragma once

#include "abstractserialoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class WriteOperation : public AbstractSerialOperation
{
    Q_OBJECT

    enum State {
        SettingUp = BasicOperationState::Ready,
        WritingData
    };

public:
    WriteOperation(QSerialPort *serialPort, const QByteArray &fileName, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    bool writeSetupCommand();
    bool writeChunk();

    bool parseError();

    QByteArray m_fileName;
    QIODevice *m_file;
    QByteArray m_receivedData;
    qint64 m_chunkSize;
};

}
}

