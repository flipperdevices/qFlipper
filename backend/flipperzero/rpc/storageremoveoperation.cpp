#include "storageremoveoperation.h"

using namespace Flipper;
using namespace Zero;

StorageRemoveOperation::StorageRemoveOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageRemoveOperation::description() const
{
    return QStringLiteral("Storage remove @%1").arg(QString(m_path));
}

void StorageRemoveOperation::onSerialPortReadyRead()
{
}

bool StorageRemoveOperation::begin()
{
    return false;
}
