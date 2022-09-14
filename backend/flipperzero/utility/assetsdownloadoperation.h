#pragma once

#include "tararchive.h"
#include "abstractutilityoperation.h"
#include "flipperzero/assetmanifest.h"

class QIODevice;
class QFile;

namespace Flipper {
namespace Zero {

class AssetsDownloadOperation : public AbstractUtilityOperation
{
    Q_OBJECT

public:
    enum State {
        CheckingExtStorage = AbstractOperation::User,
        ExtractingArchive,
        ReadingLocalManifest,
        CheckingDeviceManifest,
        ReadingDeviceManifest,
        BuildingFileLists,
        DeletingFiles,
        WritingFiles
    };

    AssetsDownloadOperation(ProtobufSession *rpc, DeviceState *deviceState, QIODevice *compressedFile, QObject *parent = nullptr);
    ~AssetsDownloadOperation();

    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void checkForExtStorage();
    void extractArchive();
    void readLocalManifest();
    void checkForDeviceManifest();
    void readDeviceManifest();
    void buildFileLists();
    void deleteFiles();
    void writeFiles();
    void cleanup();

    QIODevice *m_compressedFile;
    QFile *m_uncompressedFile;

    TarArchive *m_archive;

    AssetManifest m_localManifest;
    AssetManifest m_deviceManifest;
    bool m_isDeviceManifestPresent;

    FileNode::FileInfoList m_deleteList;
    FileNode::FileInfoList m_writeList;
};

}
}

