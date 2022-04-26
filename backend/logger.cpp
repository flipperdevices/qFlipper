#include "logger.h"

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(CATEGORY_LOGGER, "LOG")

Logger::Logger(QObject *parent):
    QObject(parent),
    m_logDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)),
    m_logFile(new QFile(this)),
    m_updateTimer(new QTimer(this)),
    m_stderr(stderr, QIODevice::WriteOnly),
    m_fileOut(m_logFile),
    m_startTime(QDateTime::currentDateTime()),
    m_logLevel(Default),
    m_maxLineCount(200),
    m_errorCount(0)
{
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(100);

    connect(m_updateTimer, &QTimer::timeout, this, &Logger::logTextChanged);

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

    const auto fileName = QStringLiteral("%1-%2.txt").arg(APP_NAME, m_startTime.toString(QStringLiteral("yyyyMMdd-hhmmss")));
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
    const auto criticalText = QStringLiteral("<font color=\"#ff1f00\">%1</font>");
    const auto timestamp = QString::number(globalLogger->m_startTime.msecsTo(QDateTime::currentDateTime()));

    // Writing everything in the file regardless of the log level
    if(globalLogger->m_logFile->isOpen()) {
        globalLogger->m_fileOut << timestamp << ' ' <<  text << Qt::endl;
    }

    const auto filterNonError = globalLogger->m_logLevel == ErrorsOnly && type != QtCriticalMsg;
    const auto filterDebug = globalLogger->m_logLevel == Terse && type == QtDebugMsg;

    if(filterNonError || filterDebug) {
        return;
    }

    globalLogger->m_stderr << timestamp << ' ' << text << Qt::endl;

    const auto filterWithoutCategory = !strcmp(context.category, "default");
    const auto filterPretty = type == QtDebugMsg;

    if(filterWithoutCategory || filterPretty) {
        return;
    }

    globalLogger->append(type == QtCriticalMsg ? criticalText.arg(text) : text);
    globalLogger->setErrorCount(globalLogger->errorCount() + (type == QtCriticalMsg ? 1 : 0));
}

const QUrl Logger::logsPath() const
{
    return QUrl::fromLocalFile(m_logDir.absolutePath());
}

const QUrl Logger::logsFile() const
{
    return QUrl::fromLocalFile(m_logFile->fileName());
}

int Logger::errorCount() const
{
    return m_errorCount;
}

void Logger::setErrorCount(int count)
{
    if(m_errorCount == count) {
        return;
    }

    m_errorCount = count;
    emit errorCountChanged();
}

QString Logger::logText() const
{
    return m_logText.join("<br/>");
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::append(const QString &line)
{
    m_logText.append(line);

    if(m_logText.size() > m_maxLineCount) {
        m_logText.removeFirst();
    }

    if(!m_updateTimer->isActive()) {
        m_updateTimer->start();
    }
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
