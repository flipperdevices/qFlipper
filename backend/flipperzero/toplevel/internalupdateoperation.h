#pragma once

#include "abstracttopleveloperation.h"

#include <QDir>

#include "flipperupdates.h"

class QFile;

namespace Flipper {
namespace Zero {

class UtilityInterface;

class InternalUpdateOperation : public Flipper::Zero::AbstractTopLevelOperation
{
    Q_OBJECT

    enum OperationState {
        FetchingFirmware = AbstractOperation::User,
        ExtractingFirmware,
        UploadingFimware,
        WaitingForUpdate
    };

public:
    InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void fetchFirmware();
    void extractFirmware();
    void uploadFirmware();
    void startUpdate();

    QFile *m_firmwareFile;
    UtilityInterface *m_utility;
    Updates::VersionInfo m_versionInfo;
};

}
}

