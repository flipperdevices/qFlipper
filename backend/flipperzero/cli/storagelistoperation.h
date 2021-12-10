#pragma once

#include "abstractserialoperation.h"
#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class StorageListOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    StorageListOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent = nullptr);
    const QString description() const override;
    const FileInfoList &files() const;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    QByteArray m_path;
    FileInfoList m_result;
};

}
}

