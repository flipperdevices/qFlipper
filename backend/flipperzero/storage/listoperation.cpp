#include "listoperation.h"

#include <QBuffer>

#include "macros.h"

#define FILE_PREFIX QByteArrayLiteral("[F]")
#define DIRECTORY_PREFIX QByteArrayLiteral("[D]")

using namespace Flipper;
using namespace Zero;

ListOperation::ListOperation(QSerialPort *serialPort, const QByteArray &dirName, QObject *parent):
    SimpleSerialOperation(serialPort, parent),
    m_dirName(dirName)
{}

const QString ListOperation::description() const
{
    return QStringLiteral("List @%1").arg(QString(m_dirName));
}

const FileInfoList &ListOperation::result() const
{
    return m_result;
}

QByteArray ListOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n\r\n>: \a");
}

QByteArray ListOperation::commandLine() const
{
    return QByteArrayLiteral("storage list \"") + m_dirName + QByteArrayLiteral("\"\r\n");
}

bool ListOperation::parseReceivedData()
{
    QBuffer buf;

    if(!buf.open(QIODevice::ReadWrite)) {
        finishWithError(buf.errorString());
        return false;
    }

    buf.write(receivedData());
    buf.seek(0);

    while(buf.canReadLine()) {
        const auto line = buf.readLine().trimmed();
        if(line.startsWith(FILE_PREFIX)) {
            parseFile(line);
        } else if(line.startsWith(DIRECTORY_PREFIX)) {
            parseDirectory(line);
        } else {}
    }

    return true;
}

void ListOperation::parseDirectory(const QByteArray &line)
{
    FileInfo info;
    info.name = line.mid(DIRECTORY_PREFIX.size() + 1);
    info.absolutePath = m_dirName + QByteArrayLiteral("/") + info.name;
    info.type = FileType::Directory;
    info.size = 0;

    m_result.append(info);
}

void ListOperation::parseFile(const QByteArray &line)
{
    const auto sizeIdx = line.lastIndexOf(' ') + 1;
    const auto nameIdx = FILE_PREFIX.size() + 1;

    FileInfo info;
    info.name = line.mid(nameIdx, sizeIdx - nameIdx - 1);
    info.absolutePath = m_dirName + QByteArrayLiteral("/") + info.name;
    info.type = FileType::RegularFile;
    info.size = line.mid(sizeIdx, line.size() - sizeIdx - 1).toLongLong(nullptr, 10);

    m_result.append(info);
}
