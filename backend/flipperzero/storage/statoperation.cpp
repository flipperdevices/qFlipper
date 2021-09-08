#include "statoperation.h"

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
*/

static size_t fromStringSize(const QByteArray &s);

StatOperation::StatOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent):
    StorageOperation(serialPort, parent),
    m_fileName(fileName),
    m_size(0),
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

size_t StatOperation::size() const
{
    return m_size;
}

StatOperation::Type StatOperation::type() const
{
    return m_type;
}

void StatOperation::onSerialPortReadyRead()
{
    if(!serialPort()->canReadLine()) {
        return;
    }

    const auto line = serialPort()->readLine();

    if(state() == State::SkippingReply) {
        setState(State::ReadingReply);

    } else if(state() == State::ReadingReply) {
        if(!parseReply(line.trimmed().toLower())) {
            finishWithError(QStringLiteral("Failed to parse the reply string"));
        } else {
            setState(State::SkippingReply);
        }
    }
}

bool StatOperation::begin()
{
    setState(State::SkippingReply);

    const auto cmdline = QByteArrayLiteral("storage stat ") + m_fileName + QByteArrayLiteral("\r\n");
    return (serialPort()->write(cmdline) == cmdline.size());
}

bool StatOperation::parseReply(const QByteArray &reply)
{
    if(reply == "storage error: file/dir not exist") {
        m_type = Type::NotFound;
    } else if(reply == "storage error: invalid name/path") {
        m_type = Type::Invalid;
    } else if(reply.contains("file, size:")) {
        const auto i = reply.indexOf(':');
        const auto tsize = reply.mid(i + 1);

        m_size = fromStringSize(tsize);
        m_type = Type::File;

    } else if(reply.contains("directory")) {
        m_type = Type::Directory;
    } else if(reply.contains("storage,")) {
        m_type = Type::Storage;
    } else {
        error_msg("Unexpected stat reply string.");
        return false;
    }

    return true;
}

static size_t fromStringSize(const QByteArray &s)
{
    if(s.endsWith("kb")) {
        return s.chopped(2).toULong() * 1024;
    } else if(s.endsWith('b')) {
        return s.chopped(1).toULong();
    } else {
        return 0;
    }
}
