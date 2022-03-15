#pragma once

#include <QDir>
#include <QUrl>
#include <QObject>
#include <QDateTime>
#include <QTextStream>
#include <QStringList>

class QFile;
class QTimer;

class Logger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl logsPath READ logsPath CONSTANT)
    Q_PROPERTY(QUrl logsFile READ logsFile CONSTANT)
    Q_PROPERTY(int errorCount READ errorCount WRITE setErrorCount NOTIFY errorCountChanged)
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)

    Logger(QObject *parent = nullptr);

public:
    enum LogLevel {
        ErrorsOnly, //< Only display qCritical messages
        Terse,      //< No qDebug messages
        Default     //< Display everything
    };

    static Logger *instance();
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    const QUrl logsPath() const;
    const QUrl logsFile() const;

    int errorCount() const;
    void setErrorCount(int count);
    QString logText() const;

    void setLogLevel(LogLevel level);

signals:
    void logTextChanged();
    void errorCountChanged();

private:
    void append(const QString &line);
    void fallbackMessageOutput(const QString &msg);
    bool removeOldFiles();

    QDir m_logDir;
    QFile *m_logFile;
    QTimer *m_updateTimer;

    QTextStream m_stderr;
    QTextStream m_fileOut;

    QDateTime m_startTime;
    LogLevel m_logLevel;
    QStringList m_logText;
    int m_maxLineCount;
    int m_errorCount;
};

#define globalLogger (Logger::instance())
