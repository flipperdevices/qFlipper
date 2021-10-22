#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>

#include "appupdater.h"
#include "flipperupdates.h"
#include "qflipperbackend.h"

class Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)
    Q_PROPERTY(AppUpdater* updater READ updater CONSTANT)

public:
    Application(int &argc, char **argv);

    static const QString commitNumber();

    AppUpdater *updater();

private:
    void initStyles();
    void initContextProperties();
    void initInstanceProperties();
    void initTranslations();
    void initQmlTypes();
    void initImports();
    void initFonts();
    void initGUI();

    AppUpdater m_updater;
    QFlipperBackend m_backend;
    QQmlApplicationEngine m_engine;
};

