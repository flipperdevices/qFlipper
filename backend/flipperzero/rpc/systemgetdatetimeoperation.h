#pragma once

#include "abstractprotobufoperation.h"

#include <QDateTime>

namespace Flipper {
namespace Zero {

class SystemGetDateTimeOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemGetDateTimeOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;
    const QDateTime &dateTime() const;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
    QDateTime m_dateTime;
};

}
}

