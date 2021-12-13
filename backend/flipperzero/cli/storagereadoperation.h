#pragma once

#include "abstractprotobufoperation.h"

#include <QByteArray>

class QIODevice;

namespace Flipper {
namespace Zero {

class StorageReadOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    StorageReadOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    QByteArray m_path;
    QIODevice *m_file;
};

}
}

