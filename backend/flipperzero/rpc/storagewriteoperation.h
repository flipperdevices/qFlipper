#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class StorageWriteOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageWriteOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;
    void onTotalBytesWrittenChanged() override;

private:
    bool begin() override;
    bool writeChunk();

    QByteArray m_path;
    QIODevice *m_file;
    qint64 m_byteCount;
};

}
}

