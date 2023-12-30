#pragma once

#include <QUrl>
#include <QTimer>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "applicationbackend.h"

class Cli : public QCoreApplication
{
    Q_OBJECT

    enum OperationType {
        NoOperation,
        List,
        Update,
        Backup,
        Restore,
        Erase,
        Firmware,
        Core2Radio,
        Core2FUS
    };

    enum OptionIndex {
        DeviceNameOption,
        DebugLevelOption,
        UpdateChannelOption
    };

public:
    Cli(int &argc, char *argv[]);
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
    void processDeviceNameOption();
    void processUpdateChannelOption();

    void beginList();
    void beginUpdate();
    void beginBackup();
    void beginRestore();
    void beginErase();
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

