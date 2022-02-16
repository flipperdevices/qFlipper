#include "storagelistoperation.h"


using namespace Flipper;
using namespace Zero;

StorageListOperation::StorageListOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path)
{}

const QString StorageListOperation::description() const
{
    return QStringLiteral("Storage list @%1").arg(QString(m_path));
}

const FileInfoList &StorageListOperation::files() const
{
    return m_result;
}

void StorageListOperation::onSerialPortReadyRead()
{
}

bool StorageListOperation::begin()
{
    return false;
}
