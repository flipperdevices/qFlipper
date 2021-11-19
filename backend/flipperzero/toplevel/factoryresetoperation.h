#pragma once

#include <QElapsedTimer>

#include "abstracttopleveloperation.h"

namespace Flipper {
namespace Zero {

class UtilityInterface;

class FactoryResetOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        ResettingDevice = AbstractOperation::User,
        Waiting
    };

public:
    FactoryResetOperation(UtilityInterface *utility, DeviceState *state, QObject *parent = nullptr);
    const QString description() const override;

private slots:
   void nextStateLogic() override;

private:
   void resetDevice();
   void wait();

   UtilityInterface *m_utility;
   QElapsedTimer m_elapsed;
};

}
}

