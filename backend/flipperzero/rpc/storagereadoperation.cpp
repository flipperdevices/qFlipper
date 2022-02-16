#include "storagereadoperation.h"

using namespace Flipper;
using namespace Zero;

StorageReadOperation::StorageReadOperation(QSerialPort *serialPort, const QByteArray &path, QIODevice *file, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path),
    m_file(file)
{}

const QString StorageReadOperation::description() const
{
    return QStringLiteral("Storage read @%1").arg(QString(m_path));
}

void StorageReadOperation::onSerialPortReadyRead()
{
}

bool StorageReadOperation::begin()
{
    return false;
}

void StorageReadOperation::rewindAndFinish()
{
}
