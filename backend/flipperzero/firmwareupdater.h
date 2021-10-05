#pragma once

#include "abstractoperationrunner.h"

namespace Flipper {
namespace Zero {

class DeviceState;
class RecoveryInterface;
class UtilityInterface;

class FirmwareUpdater : public AbstractOperationRunner
{
    Q_OBJECT

    enum class ChannelType {
        Development = 0,
        ReleaseCandidate,
        Release
    };

    Q_PROPERTY(bool isReady READ isReady NOTIFY updateInfoChanged)
    Q_PROPERTY(bool canUpdate READ canUpdate NOTIFY updateInfoChanged)
    Q_PROPERTY(bool canRollback READ canRollback NOTIFY updateInfoChanged)
    Q_PROPERTY(bool canChangeChannel READ canChangeChannel NOTIFY updateInfoChanged)

public:
    FirmwareUpdater(DeviceState *state, QObject *parent = nullptr);

    bool isReady() const;

    bool canUpdate() const;
    bool canRollback() const;
    bool canChangeChannel() const;

public slots:
    void fullUpdate();

signals:
    void updateInfoChanged();

private:
    static const QString &channelName(ChannelType channelType);
    const QString &branchToChannelName() const;

    DeviceState *m_state;
    RecoveryInterface *m_recovery;
    UtilityInterface *m_utility;
};

}
}

