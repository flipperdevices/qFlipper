#pragma once

#include "abstractutilityoperation.h"

#include <QUrl>
#include <QDir>

namespace Flipper {
namespace Zero {

class ChecksumVerifyOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        ReadingFileList = AbstractOperation::User,
        VerifyingMd5Sum
    };

    struct FileListElement {
        QFileInfo fileInfo;
        QDir topmostDir;
    };

public:

    ChecksumVerifyOperation(ProtobufSession *rpc, DeviceState *deviceState, const QList<QUrl> &urlsToCheck,
                            const QByteArray &remoteRootPath, QObject *parent = nullptr);
    const QString description() const override;
    const QList<QUrl> &changedUrls() const;

private slots:
    void nextStateLogic() override;

private:
    void readFileList();
    void verifyMd5Sums();

    static const QByteArray calculateMd5Sum(const QFileInfo &fileInfo);

    QByteArray m_remoteRootPath;
    QList<QUrl> m_urlsToCheck;
    QList<FileListElement> m_flatFileList;
    QList<QUrl> m_changedUrls;
};

}
}

