#pragma once

#include "abstractmessagingoperaton.h"

namespace Flipper {
namespace Zero {

class Recovery;

class RecoveryOperation : public AbstractMessagingOperaton
{
    Q_OBJECT

public:
    RecoveryOperation(Recovery *recovery, QObject *parent = nullptr);
    virtual ~RecoveryOperation() {}

    void start() override;
    void finish() override;

public slots:
    void onDeviceOnlineChanged(bool isOnline);

protected:
    Recovery *recovery() const;

protected slots:
    virtual void transitionToNextState() = 0;

private:
    Recovery *m_recovery;
};

}
}

