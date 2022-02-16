#include "storageinfooperation.h"

using namespace Flipper;
using namespace Zero;

StorageInfoOperation::StorageInfoOperation(QSerialPort *serialPort, const QByteArray &path, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_path(path),
    m_isPresent(false),
    m_sizeFree(0),
    m_sizeTotal(0)
{}

const QString StorageInfoOperation::description() const
{
    return QStringLiteral("Storage info @%1").arg(QString(m_path));
}

bool StorageInfoOperation::isPresent() const
{
    return m_isPresent;
}

quint64 StorageInfoOperation::sizeFree() const
{
    return m_sizeFree;
}

quint64 StorageInfoOperation::sizeTotal() const
{
    return m_sizeTotal;
}

void StorageInfoOperation::onSerialPortReadyRead()
{
}

bool StorageInfoOperation::begin()
{
    return false;
}
