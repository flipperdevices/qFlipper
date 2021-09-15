#include "readoperation.h"

#include <QSerialPort>

#include "macros.h"

#define SIZE_HEADER QByteArrayLiteral("Size: ")
#define FINISH_PROMPT QByteArrayLiteral("\r\n\r\n>: ")

using namespace Flipper;
using namespace Zero;

ReadOperation::ReadOperation(QSerialPort *serialPort, const QByteArray &fileName, QObject *parent):
    AbstractSerialOperation(serialPort, parent),
    m_size(0),
    m_lineCount(0),
    m_fileName(fileName)
{}

const QString ReadOperation::description() const
{
    return QStringLiteral("Read @%1").arg(QString(m_fileName));
}

const QByteArray &ReadOperation::result() const
{
    return m_receivedData;
}

void ReadOperation::onSerialPortReadyRead()
{
    startTimeout();

    if(state() == State::SettingUp) {
        if(!serialPort()->canReadLine()) {
            return;
        }

        const auto line = serialPort()->readLine();

        if(++m_lineCount < 2) {
            return;

        } else if(line.startsWith(SIZE_HEADER)) {
            bool success;
            m_size = line.mid(SIZE_HEADER.size()).toLongLong(&success, 10);

            if(!success) {
                finishWithError(QStringLiteral("Unexpected reply"));
                return;
            }

            setState(State::ReceivingData);
            m_receivedData += serialPort()->readAll();

        } else if(line.startsWith(QByteArrayLiteral("Storage error:"))) {
            serialPort()->clear();
            finishWithError(line.trimmed());

        } else {
            finishWithError(QStringLiteral("Unexpected number of reply header lines"));
        }

    } else if(state() == State::ReceivingData) {
        m_receivedData += serialPort()->readAll();

        if(m_receivedData.endsWith(FINISH_PROMPT)) {
            m_receivedData.chop(FINISH_PROMPT.size());

            if(m_size != m_receivedData.size()) {
                finishWithError(QStringLiteral("Size mismatch: expected %1, received %2").arg(m_size, m_receivedData.size()));
            } else {
                finish();
            }
        }
    }
}

bool ReadOperation::begin()
{
    const auto cmdLine = QByteArrayLiteral("storage read ") + m_fileName + QByteArrayLiteral("\r");
    const auto success = (serialPort()->write(cmdLine) == cmdLine.size()) && serialPort()->flush();

    if(success) {
        setState(State::SettingUp);
        startTimeout();
    }

    return success;
}
