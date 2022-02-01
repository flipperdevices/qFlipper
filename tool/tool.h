#pragma once

#include <QCoreApplication>

#include "applicationbackend.h"

class Tool : public QCoreApplication
{
    Q_OBJECT

public:
    Tool(int argc, char *argv[]);
    ~Tool();

private slots:
    void onDefaultDeviceChanged();
    void onBackendStateChanged();

private:
    void initConnections();
    void initLogger();

    ApplicationBackend m_backend;
};

