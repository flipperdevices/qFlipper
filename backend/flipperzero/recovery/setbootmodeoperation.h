#pragma once

#include "abstractrecoveryoperation.h"

namespace Flipper {
namespace Zero {

class SetBootModeOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum OperationState {
        WaitingForBoot = AbstractOperation::User
    };

public:
    SetBootModeOperation(Recovery *recovery, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;
    void onOperationTimeout() override;

private:
    virtual int bootMode() const = 0;
    virtual const QString typeString() const = 0;

    void setBootMode();
};

class SetRecoveryBootOperation : public SetBootModeOperation
{
    Q_OBJECT

public:
    SetRecoveryBootOperation(Recovery *recovery, QObject *parent = nullptr);

private:
    int bootMode() const override;
    const QString typeString() const override;
};

class SetOSBootOperation : public SetBootModeOperation
{
    Q_OBJECT

public:
    SetOSBootOperation(Recovery *recovery, QObject *parent = nullptr);

private:
    int bootMode() const override;
    const QString typeString() const override;
};

}
}

