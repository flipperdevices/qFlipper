#include "storagemkdiroperation.h"

using namespace Flipper;
using namespace Zero;

StorageMkdirOperation::StorageMkdirOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageMkdirOperation::description() const
{
    return QStringLiteral("Storage mkdir @%1").arg(QString(m_path));
}

void StorageMkdirOperation::onSerialPortReadyRead()
{
}

bool StorageMkdirOperation::begin()
{
    return false;
}
