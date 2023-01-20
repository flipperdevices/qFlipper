#pragma once

#include "abstractutilityoperation.h"

namespace Flipper {
namespace Zero {

class PathCreateOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum OperationState {
        CheckingRootElement = AbstractOperation::User,
        CheckingRemotePath,
        CreatingRemotePath,
    };

public:
    PathCreateOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;
    bool pathExists() const;

private:
    void nextStateLogic() override;

    void checkRootElement();
    void checkRemotePath();
    void createRemotePath();

    QByteArray m_remotePath;
    QByteArrayList m_remotePathElements;

    int m_currentElementIndex;
    int m_lastExistingElementIndex;
};

}
}

