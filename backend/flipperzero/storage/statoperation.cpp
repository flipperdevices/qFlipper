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

static quint64 fromStringSize(const QByteArray &s);

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

quint64 StatOperation::size() const
{
    return m_size;
}

StatOperation::Type StatOperation::type() const
{
    return m_type;
}

void StatOperation::onSerialPortReadyRead()
{
    m_receivedData += serialPort()->readAll();

    if(m_receivedData.endsWith(">: \a")) {
        stopTimeout();

        parseReceivedData();

    } else {
        startTimeout();
    }
}

void StatOperation::onOperationTimeout()
{
    finishWithError(QStringLiteral("Operation timeout"));
}

bool StatOperation::begin()
{
    const auto cmdline = QByteArrayLiteral("storage stat ") + m_fileName + QByteArrayLiteral("\r\n");
    const auto success = serialPort()->write(cmdline) == cmdline.size();

    if(success) {
        startTimeout(1000);
    }

    return success;
}

bool StatOperation::parseReceivedData()
{
    const auto lines = m_receivedData.split('\n');
    check_return_bool(lines.size() == 4, "Wrong reply line count.");

    const auto reply = lines[1].trimmed();
    qDebug() << reply;
//    if(reply == "storage error: file/dir not exist") {
//        m_type = Type::NotFound;
//    } else if(reply == "storage error: invalid name/path") {
//        m_type = Type::Invalid;
//    } else if(reply.contains("file, size:")) {
//        const auto i = reply.indexOf(':');
//        const auto tsize = reply.mid(i + 1);

//        m_size = fromStringSize(tsize);
//        m_type = Type::File;

//    } else if(reply.contains("directory")) {
//        m_type = Type::Directory;
//    } else if(reply.contains("storage,")) {
//        m_type = Type::Storage;
//    } else {
//        error_msg("Unexpected stat reply string.");
//        return false;
//    }

    return true;
}

static quint64 fromStringSize(const QByteArray &s)
{
    if(s.endsWith("kb")) {
        return s.chopped(2).toULongLong() * 1024;
    } else if(s.endsWith('b')) {
        return s.chopped(1).toULongLong();
    } else {
        return 0;
    }
}
