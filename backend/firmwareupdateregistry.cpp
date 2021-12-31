#include "firmwareupdateregistry.h"

#include "preferences.h"

using namespace Flipper;

FirmwareUpdateRegistry::FirmwareUpdateRegistry(const QString &directoryUrl, QObject *parent):
    UpdateRegistry(directoryUrl, parent)
{
    connect(globalPrefs, &Preferences::firmwareUpdateChannelChanged, this, &UpdateRegistry::latestVersionChanged);
}

const QString FirmwareUpdateRegistry::updateChannel() const
{
    return globalPrefs->firmwareUpdateChannel();
}
