#pragma once

#include <QApplication>
#include <QQmlApplicationEngine>

#include "qflipperbackend.h"

class Application : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(QString name READ applicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString version READ applicationVersion NOTIFY applicationVersionChanged)
    Q_PROPERTY(QString commit READ commitNumber CONSTANT)

public:
    Application(int &argc, char **argv);

    static const QString commitNumber();

private:
    void initStyles();
    void initContextProperties();
    void initInstanceProperties();
    void initTranslations();
    void initQmlTypes();
    void initGUI();

    QFlipperBackend m_backend;
    QQmlApplicationEngine m_engine;
};

