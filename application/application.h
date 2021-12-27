#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>

#include "applicationupdater.h"
#include "applicationbackend.h"

class Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)
    Q_PROPERTY(ApplicationUpdater* updater READ updater CONSTANT)
    Q_PROPERTY(bool dangerousFeatures READ isDangerousFeaturesEnabled CONSTANT)

public:
    Application(int &argc, char **argv);
    ~Application();

    static const QString commitNumber();
    ApplicationUpdater *updater();

    bool isDangerousFeaturesEnabled() const;

private:
    void initLogger();
    void initStyles();
    void initContextProperties();
    void initTranslations();
    void initQmlTypes();
    void initImports();
    void initFonts();
    void initGUI();

    ApplicationUpdater m_updater;
    ApplicationBackend m_backend;
    QQmlApplicationEngine m_engine;

    bool m_dangerFeaturesEnabled;
};

