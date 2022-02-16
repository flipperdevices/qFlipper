#pragma once

#include "abstractserialoperation.h"

namespace Flipper {
namespace Zero {

class StorageMkdirOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:

    StorageMkdirOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;

    QByteArray m_path;
};

}
}

