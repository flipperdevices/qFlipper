#pragma once

#include <QDir>
#include <QUrl>
#include <QObject>
#include <QDateTime>
#include <QTextStream>

class QFile;

class Logger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl logsPath READ logsPath CONSTANT)
    Q_PROPERTY(int errorCount READ errorCount WRITE setErrorCount NOTIFY errorCountChanged)

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

    int errorCount() const;
    void setErrorCount(int count);

    void setLogLevel(LogLevel level);

signals:
    void messageArrived(const QString&);
    void errorCountChanged();

private:
    void timerEvent(QTimerEvent *e) override;
    void fallbackMessageOutput(const QString &msg);
    bool removeOldFiles();

    QDir m_logDir;
    QFile *m_logFile;

    QTextStream m_stderr;
    QTextStream m_fileOut;

    QDateTime m_startTime;
    LogLevel m_logLevel;
    QString m_buffer;
    int m_errorCount;
};

#define globalLogger (Logger::instance())
