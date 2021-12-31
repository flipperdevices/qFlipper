#pragma once

#include "updateregistry.h"

namespace Flipper {

class FirmwareUpdateRegistry : public UpdateRegistry
{
    Q_OBJECT

public:
    FirmwareUpdateRegistry(const QString &directoryUrl, QObject *parent = nullptr);

private:
    const QString updateChannel() const override;
};

}

