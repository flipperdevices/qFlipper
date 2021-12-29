#pragma once

#include "updateregistry.h"

class ApplicationUpdateRegistry : public Flipper::UpdateRegistry
{
    Q_OBJECT
public:
    ApplicationUpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);

private:
    const QString updateChannel() const override;
};


