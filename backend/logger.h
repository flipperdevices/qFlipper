#pragma once

#include <QDir>
#include <QObject>

class QFile;

class Logger : public QObject
{
    Q_OBJECT

    Logger(QObject *parent = nullptr);

public:
    static Logger *instance();
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void messageArrived(const QString&);

private:
    QDir m_logDir;
    QFile *m_logFile;
};

#define globalLogger (Logger::instance())

