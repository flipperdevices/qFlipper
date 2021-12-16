#pragma once

#include <QApplication>

class ApplicationUpdater;
class ApplicationBackend;
class QQmlApplicationEngine;

class Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)
    Q_PROPERTY(ApplicationUpdater* updater READ updater CONSTANT)

public:
    Application(int &argc, char **argv);
    ~Application();

    static const QString commitNumber();
    ApplicationUpdater *updater();

private:
    void initLogger();
    void initStyles();
    void initContextProperties();
    void initTranslations();
    void initQmlTypes();
    void initImports();
    void initFonts();
    void initGUI();

    ApplicationUpdater *m_updater;
    ApplicationBackend *m_backend;
    QQmlApplicationEngine *m_engine;
};

