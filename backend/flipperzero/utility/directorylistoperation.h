#pragma once

#include "abstractutilityoperation.h"

#include "fileinfo.h"

namespace Flipper {
namespace Zero {

class DirectoryListOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        CheckingSDCard = AbstractOperation::User,
        ListingDirectory
    };

public:
    DirectoryListOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;

    bool isRoot() const;
    bool isSDCardPath() const;
    bool isSDCardPresent() const;

    const FileInfoList &files() const;

private:
    void nextStateLogic() override;

    void checkSDCard();
    void listDirectory();

    QByteArray m_remotePath;
    bool m_isSDCardPresent;
    FileInfoList m_files;
};

}
}

