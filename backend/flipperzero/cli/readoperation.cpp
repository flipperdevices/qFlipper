#include "readoperation.h"

#include <QSerialPort>

#define READY_PROMPT QByteArrayLiteral("\r\nReady?\r\n")
#define FINISH_PROMPT QByteArrayLiteral("\r\n\r\n>: ")

#define READY_PROMPT_LINE_COUNT 5
#define FINISH_PROMPT_LINE_COUNT 4

#define CHUNK_SIZE 512

using namespace Flipper;
using namespace Zero;

ReadOperation::ReadOperation(QSerialPort *serialPort, const QByteArray &fileName, QIODevice *file, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_size(0),
    m_fileName(fileName),
    m_file(file)
{}

const QString ReadOperation::description() const
{
    return QStringLiteral("Read @%1").arg(QString(m_fileName));
}

void ReadOperation::onSerialPortReadyRead()
{
    startTimeout();

    m_receivedData += serialPort()->readAll();

    if(state() == State::SettingUp) {
        if(m_receivedData.endsWith(READY_PROMPT)) {
            if(!parseSetupReply()) {
                finish();
            } else {
                setState(State::ReceivingData);
                serialPort()->write("\n");
            }

            m_receivedData.clear();
        } else if(m_receivedData.endsWith(FINISH_PROMPT)) {
            parseError();
            finish();
        }

    } else if(state() == State::ReceivingData) {
        if(m_receivedData.endsWith(READY_PROMPT)) {
            m_file->write(m_receivedData.chopped(READY_PROMPT.size()));
            m_receivedData.clear();
            serialPort()->write("\n");

        } else if(m_receivedData.endsWith(FINISH_PROMPT)) {
            m_file->write(m_receivedData.chopped(FINISH_PROMPT.size()));
            m_file->seek(0);
            finish();
        }
    }
}

bool ReadOperation::begin()
{
    const auto cmdLine = QByteArrayLiteral("storage read_chunks \"") + m_fileName + QByteArrayLiteral("\" ") +
                         QByteArray::number(CHUNK_SIZE, 10) + QByteArrayLiteral("\r");
    const auto success = (serialPort()->write(cmdLine) == cmdLine.size()) && serialPort()->flush();

    if(success) {
        setState(State::SettingUp);
        startTimeout();
    }

    return success;
}

bool ReadOperation::parseError()
{
    const auto lines = m_receivedData.split('\n');

    if(lines.size() != FINISH_PROMPT_LINE_COUNT) {
        setError(QStringLiteral("Unexpected error message line count"));
        return false;
    }

    const auto &msg = lines.at(1).trimmed();

    if(!msg.startsWith("Storage error:")) {
        setError(QStringLiteral("Unexpected error message format"));
        return false;
    }

    setError(msg);
    return true;
}

bool ReadOperation::parseSetupReply()
{
    const auto lines = m_receivedData.split('\n');
    if(lines.size() != READY_PROMPT_LINE_COUNT) {
        setError(QStringLiteral("Unexpected setup message line count"));
        return false;
    }

    const auto &sizeMsg = lines.at(1);
    if(!sizeMsg.startsWith("Size:")) {
        setError(QStringLiteral("Unexpected setup message format"));
        return false;
    }

    return parseSize(sizeMsg);
}

bool ReadOperation::parseSize(const QByteArray &s)
{
    const auto tokens = s.split(':');

    if(tokens.size() != 2) {
        setError(QStringLiteral("Unexpected size message format"));
        return false;
    }

    bool success;
    m_size = tokens.at(1).toLongLong(&success, 10);

    return success;
}
