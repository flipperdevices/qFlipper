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

    Logger(QObject *parent = nullptr);

public:
    static Logger *instance();
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    const QUrl logsPath() const;

signals:
    void messageArrived(const QString&);

private:
    void fallbackMessageOutput(const QString &msg);
    bool removeOldFiles();

    QDir m_logDir;
    QFile *m_logFile;

    QTextStream m_stderr;
    QTextStream m_fileOut;

    QDateTime m_startTime;
};

#define globalLogger (Logger::instance())
