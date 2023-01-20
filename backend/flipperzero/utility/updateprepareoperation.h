#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class UpdatePrepareOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        CheckingRemotePath = AbstractOperation::User,
        CreatingRemotePath,
        CheckingUpdateDirectory,
        CreatingUpdateDirectory,
    };

public:
    UpdatePrepareOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &updateDirName, const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;
    bool needsCompleteUpload() const;

private:
    void nextStateLogic() override;

    void checkDirectory(const QByteArray &remotePath, OperationState altState);
    void createDirectory(const QByteArray &remotePath, OperationState altState);

    void checkRemotePath();
    void createRemotePath();
    void checkUpdateDirectory();
    void createUpdateDirectory();

    QByteArray m_updateDirName;
    QByteArray m_remotePath;

    bool m_needsCompleteUpload;
};

}
}

