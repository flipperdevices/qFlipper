#pragma once

#include "abstractoperation.h"

namespace Flipper {
namespace Zero {

class Recovery;

class RecoveryOperation : public AbstractOperation
{
    Q_OBJECT

public:
    RecoveryOperation(Recovery *recovery, QObject *parent = nullptr);
    virtual ~RecoveryOperation();

    void start() override;
    void finish() override;

signals:

public slots:
    void onDeviceOnline();
    void onDeviceOffline();

protected:
    Recovery *recovery() const;

protected slots:
    virtual void transitionToNextState() = 0;

private:
    Recovery *m_recovery;
};

}
}

