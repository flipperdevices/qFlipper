#pragma once

#include "abstractprotobufoperation.h"

#include <QHash>
#include <QByteArray>

#include "flipperzero/deviceinfo.h"

namespace Flipper {
namespace Zero {

class SystemDeviceInfoOperation : public AbstractProtobufOperation
{
    Q_OBJECT

public:
    SystemDeviceInfoOperation(QSerialPort *serialPort, QObject *parent = nullptr);
    const QString description() const override;
    const QByteArray result(const QByteArray &key) const;

private slots:
    void onSerialPortReadyRead() override;

private:
    bool begin() override;
    QHash<QByteArray, QByteArray> m_data;
};

}
}

