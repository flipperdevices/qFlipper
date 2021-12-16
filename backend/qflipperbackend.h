#pragma once

#include <QObject>

#include "deviceregistry.h"
#include "updateregistry.h"

class QFlipperBackend : public QObject
{
    Q_OBJECT

public:
    QFlipperBackend(QObject *parent = nullptr);
    ~QFlipperBackend();

    Flipper::DeviceRegistry deviceRegistry;
    Flipper::FirmwareUpdates firmwareUpdates;
    Flipper::ApplicationUpdates applicationUpdates;
};
