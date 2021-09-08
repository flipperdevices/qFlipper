#pragma once

#include "abstractoperation.h"

class QSerialPort;

namespace Flipper {
namespace Zero {

class StorageOperation : public AbstractOperation
{
    Q_OBJECT

public:
    StorageOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    virtual ~StorageOperation();

    void start() override;
    void finish() override;

protected:
    QSerialPort *serialPort() const;

private slots:
    virtual void onSerialPortReadyRead() = 0;
    void onSerialPortError();

private:
    virtual bool begin() = 0;

    QSerialPort *m_serialPort;
};

}
}

