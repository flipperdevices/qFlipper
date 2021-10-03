#pragma once

#include "abstractoperation.h"

namespace Flipper {
namespace Zero {

class Recovery;
class DeviceState;

class RecoveryOperation : public AbstractOperation
{
    Q_OBJECT

public:
    RecoveryOperation(Recovery *recovery, QObject *parent = nullptr);
    virtual ~RecoveryOperation() {}

    void start() override;
    void finish() override;

protected:
    Recovery *recovery() const;
    DeviceState *deviceState() const;

private slots:
    void onDeviceOnlineChanged();
    virtual void doNextOperationState() = 0;

private:
    Recovery *m_recovery;
};

}
}

