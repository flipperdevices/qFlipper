#pragma once

#include <QObject>
#include <QtPlugin>

#include "protobufinterface.h"

class ProtobufPlugin : public QObject, public ProtobufInterface
{
    Q_OBJECT

public:
    ProtobufPlugin(QObject *parent = nullptr);
};

