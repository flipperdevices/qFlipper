#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class UpdatePrepareOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        CheckingRemoteDirectory = AbstractOperation::User,
        CreatingRemoteDirectory,
        CheckingUpdateDirectory,
    };

public:
    UpdatePrepareOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &updateDirName, const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;
    bool updateDirectoryExists() const;

private:
    void nextStateLogic() override;

    void checkRemoteDirectory();
    void createRemoteDirectory();
    void checkUpdateDirectory();

    QByteArray m_updateDirName;
    QByteArray m_remotePath;
    bool m_updateDirExists;
};

}
}

