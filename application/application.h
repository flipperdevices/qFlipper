#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>

#include "applicationupdater.h"
#include "applicationbackend.h"
#include "applicationupdateregistry.h"

class Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)

    Q_PROPERTY(bool dangerousFeatures READ isDangerousFeaturesEnabled CONSTANT)
    Q_PROPERTY(UpdateStatus updateStatus READ updateStatus NOTIFY updateStatusChanged)

public:
    enum class UpdateStatus {
        NoUpdates,
        Checking,
        CanUpdate
    };

    Q_ENUM(UpdateStatus)

    Application(int &argc, char **argv);
    ~Application();

    static const QString commitNumber();
    bool isDangerousFeaturesEnabled() const;
    UpdateStatus updateStatus() const;

    Q_INVOKABLE void selfUpdate();
    Q_INVOKABLE void checkForUpdates();

signals:
    void updateStatusChanged();

private slots:
    void onLatestVersionChanged();

private:
    void initConnections();
    void initLogger();
    void initStyles();
    void initContextProperties();
    void initTranslations();
    void initQmlTypes();
    void initImports();
    void initFonts();
    void initGUI();

    void setUpdateStatus(UpdateStatus newUpdateStatus);

    ApplicationUpdater m_updater;
    ApplicationUpdateRegistry m_updateRegistry;
    ApplicationBackend m_backend;
    QQmlApplicationEngine m_engine;

    bool m_dangerFeaturesEnabled;
    UpdateStatus m_updateStatus;
};

