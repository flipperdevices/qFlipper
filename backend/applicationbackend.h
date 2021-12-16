#pragma once

#include <QObject>

#include "deviceregistry.h"
#include "updateregistry.h"

class ApplicationBackend : public QObject
{
    Q_OBJECT

public:
    ApplicationBackend(QObject *parent = nullptr);
    ~ApplicationBackend();

public:
    // TODO: Make these private
    Flipper::DeviceRegistry deviceRegistry;
    Flipper::FirmwareUpdates firmwareUpdates;
    Flipper::ApplicationUpdates applicationUpdates;
};
