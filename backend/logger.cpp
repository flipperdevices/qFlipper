#include "logger.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QStandardPaths>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(CATEGORY_LOGGER, "LOGGER")

Logger::Logger(QObject *parent):
    QObject(parent),
    m_logDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)),
    m_logFile(new QFile(this))
{
    m_logDir.mkdir(APP_NAME);

    if(!m_logDir.exists(APP_NAME)) {
        qCWarning(CATEGORY_LOGGER) << "Failed to create logs directory";
        return;

    } else if(!m_logDir.cd(APP_NAME)) {
        qCWarning(CATEGORY_LOGGER) << "Failed to access logs directory";
        return;
    }

    m_logFile->setFileName(m_logDir.absoluteFilePath(QStringLiteral("%1.log").arg(APP_NAME)));

    if(!m_logFile->open(QIODevice::WriteOnly)) {
        qCWarning(CATEGORY_LOGGER).noquote() << "Failed to open log file:" << m_logFile->errorString();
    }
}

Logger *Logger::instance()
{
    static auto *logger = new Logger();
    return logger;
}

void Logger::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    const auto text = QStringLiteral("[%1] %2").arg(context.category, msg);

    QTextStream out(stderr, QIODevice::WriteOnly);

    // TODO: Distinguish between severity levels in the log file?
    switch(type) {
    case QtFatalMsg:
        break;
    case QtDebugMsg:
        break;
    case QtCriticalMsg:
        break;
    case QtInfoMsg:
    case QtWarningMsg:
        emit globalLogger->messageArrived(text);
    }

    if(globalLogger->m_logFile->isOpen()) {
        QTextStream fileOut(globalLogger->m_logFile);
        fileOut << text << Qt::endl;
    }

    out << text << Qt::endl;
}

const QUrl Logger::logsPath() const
{
    return QUrl::fromLocalFile(m_logDir.absolutePath());
}
