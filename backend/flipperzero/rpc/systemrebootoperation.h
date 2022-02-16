#pragma once

#include "abstractserialoperation.h"

namespace Flipper {
namespace Zero {

class SystemRebootOperation : public AbstractSerialOperation
{
    Q_OBJECT

public:
    enum class RebootType {
        OS,
        Recovery
    };

    SystemRebootOperation(QSerialPort *serialPort, RebootType rebootType, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void onTotalBytesWrittenChanged() override;

private:
    bool begin() override;

    RebootType m_rebootType;
    qint64 m_byteCount;
};

}
}

