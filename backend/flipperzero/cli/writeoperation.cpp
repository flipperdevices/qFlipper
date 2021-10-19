#include "writeoperation.h"

#include <QIODevice>
#include <QSerialPort>

#include "macros.h"

#define READY_PROMPT QByteArrayLiteral("\r\nReady\r\n")
#define FINISH_PROMPT QByteArrayLiteral("\r\n>: ")

#define FINISH_PROMPT_LINE_COUNT 4

#define CHUNK_SIZE 512

using namespace Flipper;
using namespace Zero;

WriteOperation::WriteOperation(QSerialPort *serialPort, const QByteArray &fileName, QIODevice *file, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_fileName(fileName),
    m_file(file)
{}

const QString WriteOperation::description() const
{
    return QStringLiteral("Write @%1").arg(QString(m_fileName));
}

void WriteOperation::onSerialPortReadyRead()
{
    m_receivedData.append(serialPort()->readAll());

    if(operationState() == State::SettingUp) {
        if(m_receivedData.endsWith(FINISH_PROMPT)) {
            parseError();
            finish();

        } else if(!m_receivedData.endsWith(READY_PROMPT)) {
            return;
        }

        setOperationState(State::WritingData);

        if(!writeChunk()) {
            finishWithError(QStringLiteral("Failed to write chunk"));
        }

    } else if(operationState() == State::WritingData) {
        if(!m_receivedData.endsWith(FINISH_PROMPT)) {
            return;
        }

        setOperationState(State::SettingUp);

        if(!m_file->bytesAvailable()) {
            finish();
        } else if(!writeSetupCommand()) {
            finishWithError(QStringLiteral("Failed to write chunk"));
        }

    } else {
        finishWithError(QStringLiteral("Unexpected data"));
    }

    m_receivedData.clear();
}

bool WriteOperation::begin()
{
    check_return_bool(m_file->bytesAvailable(), "No data is available for reading from file");

    setOperationState(State::SettingUp);
    return writeSetupCommand();
}

bool WriteOperation::writeSetupCommand()
{
    const auto bytesAvailable = m_file->bytesAvailable();
    m_chunkSize = bytesAvailable < CHUNK_SIZE ? bytesAvailable : CHUNK_SIZE;
    const auto cmdLine = QByteArrayLiteral("storage write_chunk \"") + m_fileName + QByteArrayLiteral("\" ") +
                         QByteArray::number(m_chunkSize) + QByteArrayLiteral("\r");

    const auto success = (serialPort()->write(cmdLine) == cmdLine.size()) && serialPort()->flush();

    if(success) {
        startTimeout();
    }

    return success;
}

bool WriteOperation::writeChunk()
{
    const auto data = m_file->read(m_chunkSize);
    const auto success = (serialPort()->write(data) == data.size()) && serialPort()->flush();

    if(success) {
        startTimeout();
    }

    return success;
}

bool WriteOperation::parseError()
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
