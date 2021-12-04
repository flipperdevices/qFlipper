#include "logger.h"

#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(CATEGORY_LOGGER, "LOGGER")

Logger::Logger(QObject *parent):
    QObject(parent),
    m_logDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)),
    m_logFile(new QFile(this)),
    m_stderr(stderr, QIODevice::WriteOnly),
    m_fileOut(m_logFile),
    m_startTime(QDateTime::currentDateTime())
{
    m_logDir.mkdir(APP_NAME);

    if(!m_logDir.exists(APP_NAME)) {
        fallbackMessageOutput(QStringLiteral("Failed to create logs directory"));
        return;

    } else if(!m_logDir.cd(APP_NAME)) {
        fallbackMessageOutput(QStringLiteral("Failed to access logs directory"));
        return;

    } else if(!removeOldFiles()) {
        fallbackMessageOutput(QStringLiteral("Failed to remove old files"));
        return;
    }

    const auto fileName = QStringLiteral("%1-%2.log").arg(APP_NAME, m_startTime.toString(QStringLiteral("yyyyMMdd-hhmmss")));
    const auto filePath = m_logDir.absoluteFilePath(fileName);
    m_logFile->setFileName(filePath);

    if(!m_logFile->open(QIODevice::WriteOnly)) {
        fallbackMessageOutput(QStringLiteral("Failed to open log file: %1").arg(m_logFile->errorString()));
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

    // TODO: Distinguish between severity levels in the log file?
    switch(type) {
    case QtFatalMsg:
    case QtDebugMsg:
    case QtCriticalMsg:
        break;
    case QtInfoMsg:
    case QtWarningMsg:
        if(strcmp(context.category, "default")) {
            emit globalLogger->messageArrived(text);
        }
    }

    if(globalLogger->m_logFile->isOpen()) {
        globalLogger->m_fileOut << text << Qt::endl;
    }

    globalLogger->m_stderr << text << Qt::endl;
}

const QUrl Logger::logsPath() const
{
    return QUrl::fromLocalFile(m_logDir.absolutePath());
}

void Logger::fallbackMessageOutput(const QString &msg)
{
    m_stderr << '[' << CATEGORY_LOGGER().categoryName() << "] " << msg << Qt::endl;
}

bool Logger::removeOldFiles()
{
    constexpr auto maxFileCount = 99;
    const auto files = m_logDir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
    const auto excessFileCount = files.size() - maxFileCount;

    for(auto i = 0; i < excessFileCount; ++i) {
        const auto &fileInfo = files.at(i);
        if(!m_logDir.remove(fileInfo.fileName())) {
            fallbackMessageOutput(QStringLiteral("Failed to remove file: %1").arg(fileInfo.fileName()));
            return false;
        }
    }

    return true;
}
