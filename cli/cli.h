#pragma once

#include <QUrl>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "applicationbackend.h"

class Cli : public QCoreApplication
{
    Q_OBJECT

    enum OperationType {
        NoOperation,
        DefaultAction,
        Backup,
        Restore,
        Erase,
        Wipe,
        Firmware,
        Core2Radio,
        Core2FUS
    };

    enum OptionIndex {
        DebugLevelOption = 0,
        RepeatNumberOption,
        UpdateChannelOption
    };

public:
    Cli(int argc, char *argv[]);
    ~Cli();

private slots:
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
    void processUpdateChannelOption();

    void beginDefaultAction();
    void beginBackup();
    void beginRestore();
    void beginErase();
    void beginWipe();
    void beginFirmware();
    void beginCore2Radio();
    void beginCore2FUS();

    void startPendingOperation();
    void verifyArgumentCount(int num);

    QCommandLineParser m_parser;
    QList<QCommandLineOption> m_options;
    ApplicationBackend m_backend;
    OperationType m_pendingOperation;
    QUrl m_fileParameter;
    uint32_t m_core2Address;
    int m_repeatCount;
};

