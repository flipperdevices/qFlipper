#include "removeoperation.h"

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
    return QByteArrayLiteral("\r\n\r\n>: \a");
}

QByteArray RemoveOperation::commandLine() const
{
    return QByteArrayLiteral("storage remove \"") + m_fileName + QByteArrayLiteral("\"\r\n");
}

bool RemoveOperation::parseReceivedData()
{
    const auto lines = receivedData().split('\n');

    if(lines.size() == 4) {
        const auto msg = lines.at(1).trimmed();

        if(!msg.startsWith(QByteArrayLiteral("Storage error: "))) {
            return false;
        } else if(!msg.endsWith(QByteArrayLiteral("file/dir not exist"))) {
            setError(msg);
        } else {
            info_msg(QStringLiteral("Warning: file %1 does not exist.").arg(QString(m_fileName)));
        }

        return true;

    } else if(lines.size() == 3) {
        return true;
    } else {
        return false;
    }
}
