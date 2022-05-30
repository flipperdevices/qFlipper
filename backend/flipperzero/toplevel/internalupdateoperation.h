#pragma once

#include "abstracttopleveloperation.h"

#include <QDir>

#include "flipperupdates.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;

class InternalUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingUpdate = AbstractOperation::User,
        ExtractingUpdate,
        PreparingUpdateDir,
        UploadingUpdateDir,
        WaitingForUpdate
    };

public:
    InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void fetchUpdateFile();
    void extractUpdate();
    void prepareUpdateDir();
    void uploadUpdateDir();
    void startUpdate();

    bool findAndCdToUpdateDir();

    QFile *m_updateFile;
    QDir m_updateDirectory;
    UtilityInterface *m_utility;
    Updates::VersionInfo m_versionInfo;
};

}
}

