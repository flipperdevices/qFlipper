#pragma once

#include "abstractoperationhelper.h"
#include "flipperupdates.h"

namespace Flipper {

class UpdateRegistry;
class FlipperZero;

namespace Zero {

class AbstractTopLevelHelper : public AbstractOperationHelper
{
    Q_OBJECT

public:
    enum State {
        CheckingForUpdates = AbstractOperationHelper::User,
        RunningCustomOperation,
    };

    AbstractTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent = nullptr);
    virtual ~AbstractTopLevelHelper() {}

protected:
    UpdateRegistry *updateRegistry();
    FlipperZero *device();

private slots:
    void onUpdateRegistryStateChanged();

private:
    void nextStateLogic() override;
    void checkForUpdates();

    void finishEarly(BackendError::ErrorType error, const QString &errorString);

    virtual void runCustomOperation() = 0;

    UpdateRegistry *m_updateRegistry;
    FlipperZero *m_device;
};

class UpdateTopLevelHelper : public AbstractTopLevelHelper
{
    Q_OBJECT

public:
    UpdateTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent = nullptr);

private:
    void runCustomOperation() override;
};

class RepairTopLevelHelper : public AbstractTopLevelHelper
{
    Q_OBJECT

public:
    RepairTopLevelHelper(UpdateRegistry *updateRegistry, FlipperZero *device, QObject *parent = nullptr);

private:
    void runCustomOperation() override;
};

}
}

