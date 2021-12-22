#pragma once

#include "abstractoperation.h"

class QSerialPort;

class AbstractSerialOperation : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractSerialOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    virtual ~AbstractSerialOperation() {}

    void start() override;
    void finish() override;

protected:
    QSerialPort *serialPort() const;

    qint64 totalBytesWritten() const;
    void resetTotalBytesWritten();

private slots:
    virtual void onSerialPortReadyRead();
    virtual void onTotalBytesWrittenChanged();

    void onSerialPortBytesWritten(qint64 numBytes);
    void onSerialPortError();

private:
    virtual bool begin() = 0;

    QSerialPort *m_serialPort;
    qint64 m_totalBytesWritten;
};
