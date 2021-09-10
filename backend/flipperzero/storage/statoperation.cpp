#include "statoperation.h"

#include <QRegExp>
#include <QSerialPort>

#include "macros.h"

using namespace Flipper;
using namespace Zero;

/*
>: storage stat /ext
Storage, 30327040KB total, 30326720KB free

>: storage stat /invalid
Storage error: invalid name/path

>: storage stat /int/test
Storage error: file/dir not exist

>: storage stat /ext/irda
Directory

>: storage stat /ext/irda/Tv_home.ir
File, size: 64b

>: storage stat /ext
Storage error: internal error
*/

static qint64 fromStringSize(const QByteArray &s);

StatOperation::StatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent):
    SimpleSerialOperation(serialPort, parent),
    m_fileName(fileName),
    m_size(-1),
    m_sizeFree(-1),
    m_type(Type::Invalid)
{}

const QString StatOperation::description() const
{
    return QStringLiteral("Stat @%1").arg(QString(m_fileName));
}

const QByteArray &StatOperation::fileName() const
{
    return m_fileName;
}

qint64 StatOperation::size() const
{
    return m_size;
}

qint64 StatOperation::sizeFree() const
{
    return m_sizeFree;
}

StatOperation::Type StatOperation::type() const
{
    return m_type;
}

QByteArray StatOperation::endOfMessageToken() const
{
    return QByteArrayLiteral("\r\n>: \a");
}

QByteArray StatOperation::commandLine() const
{
    return QByteArrayLiteral("storage stat ") + m_fileName + QByteArrayLiteral("\r\n");
}

bool StatOperation::parseReceivedData()
{
    const auto lines = receivedData().split('\n');
    check_return_bool(lines.size() == 4, "Wrong reply line count.");

    const auto reply = lines[1].trimmed().toLower();

    if(reply == "storage error: file/dir not exist") {
        m_type = Type::NotFound;

    } else if(reply == "storage error: invalid name/path") {
        m_type = Type::Invalid;

    } else if(reply == "storage error: internal error") {
        m_type = Type::InternalError;

    } else if(parseFileSize(reply)) {
        m_type = Type::File;

    } else if(reply.contains("directory")) {
        m_type = Type::Directory;

    } else if(parseStorageSize(reply)) {
        m_type = Type::Storage;

    } else {
        error_msg("Unexpected stat reply string.");
        return false;
    }

    return true;
}

bool StatOperation::parseFileSize(const QByteArray &data)
{
    QRegExp expr("file, size: ([0-9]+k?i?b)");

    if(!expr.exactMatch(data) || (expr.captureCount() != 1)) {
        return false;
    }

    const auto captures = expr.capturedTexts();
    m_size = fromStringSize(captures[1].toLocal8Bit());

    return m_size >= 0;
}

bool StatOperation::parseStorageSize(const QByteArray &data)
{
    QRegExp expr("storage, ([0-9]+k?i?b) total, ([0-9]+k?i?b) free");

    if(!expr.exactMatch(data) || (expr.captureCount() != 2)) {
        return false;
    }

    const auto captures = expr.capturedTexts();
    m_size = fromStringSize(captures[1].toLocal8Bit());
    m_sizeFree = fromStringSize(captures[2].toLocal8Bit());

    return (m_size >= 0) && (m_sizeFree >= 0);
}

static qint64 fromStringSize(const QByteArray &s)
{
    qint64 result = -1;
    bool cr = true;

    if(s.endsWith("kb")) {
        result = s.chopped(2).toULongLong(&cr, 10) * 1024; //TODO: fix incorrect multipliers in the firmware?
    } else if(s.endsWith("kib")) {
        result = s.chopped(3).toULongLong(&cr, 10) * 1024;
    } else if(s.endsWith('b')) {
        result = s.chopped(1).toULongLong(&cr, 10);
    }

    return cr ? result : -1;
}
