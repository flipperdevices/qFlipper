#include "writeoperation.h"

#include <QIODevice>
#include <QSerialPort>

#include "macros.h"

#define CHUNK_SIZE 512
#define READY_PROMPT QByteArrayLiteral("\r\nReady?\r\n")
#define FINISH_PROMPT QByteArrayLiteral("\r\n\r\n>: ")

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
    m_receivedData = serialPort()->readAll();

    if(state() == State::SettingUp) {
        if(m_receivedData.endsWith(FINISH_PROMPT)) {
            if(!parseError()) {
                finishWithError(QStringLiteral("Unknown error"));
            } else {
                finishWithError(QStringLiteral("Error while issuing write_chunk command"));
            }

            return;

        } else if(!m_receivedData.endsWith(READY_PROMPT)) {
            return;
        }

        setState(State::WritingData);

        if(!writeChunk()) {
            finishWithError(QStringLiteral("Failed to write chunk"));
        }

    } else if(state() == State::WritingData) {
        if(!m_receivedData.endsWith(FINISH_PROMPT)) {
            return;
        }

        setState(State::SettingUp);

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

    setState(State::SettingUp);
    return writeSetupCommand();
}

bool WriteOperation::writeSetupCommand()
{
    const auto bytesAvailable = m_file->bytesAvailable();
    m_chunkSize = bytesAvailable < CHUNK_SIZE ? bytesAvailable : CHUNK_SIZE;
    const auto cmdLine = QByteArrayLiteral("storage write_chunk ") + m_fileName + QByteArrayLiteral(" ") +
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

bool WriteOperation::parseError() const
{
    const auto lines = m_receivedData.split('\n');
    check_return_bool(lines.size() >= 4, "Unexpected error line count.");

    const auto &msg = lines.at(1);
    check_return_bool(msg.startsWith("Storage error:"), "Unexpected error message format.");

    error_msg(msg.trimmed());
    return true;
}
