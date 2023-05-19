#pragma once

#include "abstracttopleveloperation.h"

#include <QDir>
#include <QUrl>
#include <QFileInfoList>

#include "flipperupdates.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;

class FullUpdateOperation : public AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        ProvisioninigRegion = AbstractOperation::User,
        CheckingStorage,
        PreparingLocalUpdate,
        FetchingUpdate,
        ExtractingUpdate,
        ReadingUpdateFiles,
        PreparingRemoteUpdate,
        VerifyingExistingFiles,
        UploadingUpdateFiles,
        WaitingForUpdate,
    };

public:
    FullUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    FullUpdateOperation(UtilityInterface *utility, DeviceState *deviceState, const QUrl &bundleUrl, QObject *parent = nullptr);
    ~FullUpdateOperation();
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void provisionRegionData();
    void checkStorage();
    void fetchUpdateFile();
    void prepareLocalUpdate();
    void extractUpdate();
    void readUpdateFiles();
    void createUpdatePath();
    void verifyExistingFiles();
    void uploadUpdateFiles();
    void startUpdate();

    QFile *m_updateFile;
    QDir m_updateDirectory;
    QList<QUrl> m_fileUrls;
    UtilityInterface *m_utility;
    Updates::VersionInfo m_versionInfo;
};

}
}

