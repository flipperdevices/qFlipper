#pragma once

#include "abstractoperation.h"

class QSerialPort;

class AbstractSerialOperation : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractSerialOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    virtual ~AbstractSerialOperation();

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
