#pragma once

#include <QObject>

#include "flipperupdates.h"

/// BIG TODO: Remove this class

namespace Flipper {
namespace Zero {

class DeviceState;

class FirmwareUpdater : public QObject
{
    Q_OBJECT

    enum class ChannelType {
        Development = 0,
        ReleaseCandidate,
        Release
    };

public:
    FirmwareUpdater(DeviceState *state, QObject *parent = nullptr);

    Q_INVOKABLE bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    Q_INVOKABLE bool canInstall() const;

private:
    static const QString &channelName(ChannelType channelType);
    const QString &branchToChannelName() const;
    ChannelType branchToChannelType() const;

    DeviceState *m_state;
};

}
}

