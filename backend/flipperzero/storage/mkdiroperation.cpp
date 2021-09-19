#include "mkdiroperation.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

MkDirOperation::MkDirOperation(QSerialPort *serialPort, const QByteArray &dirName, QObject *parent):
    SimpleSerialOperation(serialPort, parent),
    m_dirName(dirName)
{}

const QString MkDirOperation::description() const
{
    return QStringLiteral("MkDir @%1").arg(QString(m_dirName));
}

QByteArray MkDirOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n\r\n>: ");
}

QByteArray MkDirOperation::commandLine() const
{
    return QByteArrayLiteral("storage mkdir ") + m_dirName + QByteArrayLiteral("\r\n");
}

bool MkDirOperation::parseReceivedData()
{
    const auto lines = receivedData().split('\n');

    if(lines.size() == 4) {
        const auto msg = lines.at(1).trimmed();

        if(!msg.startsWith(QByteArrayLiteral("Storage error: "))) {
            return false;
        } else if(!msg.endsWith(QByteArrayLiteral("file/dir already exist"))) {
            setError(msg);
        } else {
            info_msg(QStringLiteral("Warning: directory %1 already exists.").arg(QString(m_dirName)));
        }

        return true;

    } else if(lines.size() == 3) {
        return true;
    } else {
        return false;
    }
}
