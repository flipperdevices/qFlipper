#include "removeoperation.h"

//>: storage remove /ext/test.txt

//>: storage remove /ext/test.txt
//Storage error: file/dir not exist

//>:

#include "macros.h"

using namespace Flipper;
using namespace Zero;

RemoveOperation::RemoveOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent):
    SimpleSerialOperation(serialPort, parent),
    m_fileName(fileName)
{}

const QString RemoveOperation::description() const
{
    return QStringLiteral("Remove @%1").arg(QString(m_fileName));
}

QByteArray RemoveOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n>: \a");
}

QByteArray RemoveOperation::commandLine() const
{
    return QByteArrayLiteral("storage remove ") + m_fileName + QByteArrayLiteral("\r\n");
}

bool RemoveOperation::parseReceivedData()
{
    qDebug() << "===========" << receivedData();
    return true;
}
