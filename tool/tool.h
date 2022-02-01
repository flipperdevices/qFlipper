#pragma once

#include <QCoreApplication>
#include <QCommandLineParser>

#include "applicationbackend.h"

class Tool : public QCoreApplication
{
    Q_OBJECT

    enum OperationType {
        NoOperation,
        FullUpdate,
        FullRepair
    };

public:
    Tool(int argc, char *argv[]);
    ~Tool();

private slots:
    void onCurrentDeviceChanged();
    void onBackendStateChanged();
    void onUpdateStateChanged();

private:
    void initConnections();
    void initLogger();
    void initParser();

    void processOptions();
    void processArguments();

    void processDebugLevelOption();
    void processRepeatNumberOption();

    void beginUpdate();
    void beginRepair();

    void startPendingOperation();

    QCommandLineParser m_parser;
    QList<QCommandLineOption> m_options;
    ApplicationBackend m_backend;
    OperationType m_pendingOperation;
    int m_repeatCount;
};

