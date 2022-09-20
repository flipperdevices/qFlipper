#pragma once

#include "abstractutilityoperation.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>

namespace Flipper {
namespace Zero {

class FilesUploadOperation : public AbstractUtilityOperation
{
    Q_OBJECT

    enum State {
        ReadingFileList = AbstractOperation::User,
        WritingFiles
    };

    struct FileListElement {
        QFileInfo fileInfo;
        QDir topmostDir;
    };

public:
    FilesUploadOperation(ProtobufSession *rpc, DeviceState *deviceState, const QList<QUrl> &fileUrls,
                         const QByteArray &remotePath, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void readFileList();
    void writeFiles();

    QByteArray m_remotePath;
    QList<QUrl> m_urlList;
    QList<FileListElement> m_fileList;
    qint64 m_totalSize;
};

}
}

