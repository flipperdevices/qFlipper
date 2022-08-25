#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>

#include "qtsingleapplication/qtsingleapplication.h"

#include "applicationupdater.h"
#include "applicationbackend.h"
#include "systemfiledialog.h"
#include "applicationupdateregistry.h"

class Application : public QtSingleApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)

    Q_PROPERTY(ApplicationUpdater* updater READ updater CONSTANT)

    Q_PROPERTY(bool isDeveloperMode READ isDeveloperMode CONSTANT)
    Q_PROPERTY(UpdateStatus updateStatus READ updateStatus NOTIFY updateStatusChanged)

    enum OptionIndex {
        DeveloperModeOption = 0,
    };

public:
    enum class UpdateStatus {
        NoUpdates,
        Checking,
        CanUpdate
    };

    Q_ENUM(UpdateStatus)

    Application(int &argc, char **argv);
    ~Application();

    ApplicationUpdater *updater();

    static const QString commitNumber();
    bool isDeveloperMode() const;
    UpdateStatus updateStatus() const;

    Q_INVOKABLE void selfUpdate();
    Q_INVOKABLE void checkForUpdates();

signals:
    void updateStatusChanged();

private slots:
    void onMessageReceived();
    void onLatestVersionChanged();
    void onCurrentDeviceChanged();

private:
    void initCommandOptions();
    void initConnections();
    void initLogger();
    void initStyles();
    void initTranslations();
    void initQmlTypes();
    void initImports();
    void initFonts();
    void initGUI();

    void setUpdateStatus(UpdateStatus newUpdateStatus);

    ApplicationUpdater m_updater;
    ApplicationUpdateRegistry m_updateRegistry;
    SystemFileDialog m_fileDialog;
    ApplicationBackend m_backend;
    QQmlApplicationEngine m_engine;

    bool m_isDeveloperMode;
    UpdateStatus m_updateStatus;
};

