#include "applicationupdateregistry.h"

#include "preferences.h"

using namespace Flipper;

ApplicationUpdateRegistry::ApplicationUpdateRegistry(const QString &directoryUrl, QObject *parent):
    UpdateRegistry(directoryUrl, parent)
{
    connect(globalPrefs, &Preferences::applicationUpdateChannelChanged, this, &UpdateRegistry::latestVersionChanged);
}

const QString ApplicationUpdateRegistry::updateChannel() const
{
    return globalPrefs->applicationUpdateChannel();
}
