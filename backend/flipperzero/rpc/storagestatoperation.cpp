#include "storagestatoperation.h"

using namespace Flipper;
using namespace Zero;

StorageStatOperation::StorageStatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_fileName(fileName),
    m_isPresent(false),
    m_size(0),
    m_type(Type::Invalid)
{}

const QString StorageStatOperation::description() const
{
    return QStringLiteral("Storage stat @%1").arg(QString(m_fileName));
}

const QByteArray &StorageStatOperation::fileName() const
{
    return m_fileName;
}

bool StorageStatOperation::isPresent() const
{
    return m_isPresent;
}

quint64 StorageStatOperation::size() const
{
    return m_size;
}

StorageStatOperation::Type StorageStatOperation::type() const
{
    return m_type;
}

void StorageStatOperation::onSerialPortReadyRead()
{
}

bool StorageStatOperation::begin()
{
    return false;
}
