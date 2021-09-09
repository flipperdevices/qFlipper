#pragma once

#include <QObject>
#include <QByteArray>

#include "failable.h"

class QTimer;
class QSerialPort;

namespace Flipper {
namespace Zero {

class MOTDSkipper : public QObject, public Failable
{
    Q_OBJECT

public:
    MOTDSkipper(QSerialPort *serialPort, QObject *parent = nullptr);

signals:
    void finished();

private slots:
    void begin();
    void onSerialPortReadyRead();
    void onSerialPortErrorOccured();
    void onResponseTimeout();

private:
    void finish();
    void finishWithError(const QString &errorMsg);

    QTimer *m_responseTimer;
    QSerialPort *m_serialPort;
    QByteArray m_receivedData;
};

}
}

