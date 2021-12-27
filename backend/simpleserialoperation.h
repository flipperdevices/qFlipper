#pragma once

#include "abstractserialoperation.h"

#include <QByteArray>

class SimpleSerialOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    enum Flags {
        DTR = (1 << 0),
        RTS = (1 << 2)
    };

    SimpleSerialOperation(QSerialPort *serialPort, QObject *parent = nullptr);

protected:
    const QByteArray &receivedData() const;

    virtual QByteArray endOfMessageToken() const = 0;
    virtual QByteArray commandLine() const;
    virtual uint32_t flags() const;

    virtual bool parseReceivedData();

private slots:
    void onSerialPortReadyRead() override;
    void onOperationTimeout() override;

private:
    bool begin() override;

    QByteArray m_receivedData;
};
