#pragma once

#include <QQueue>

#include "tararchive.h"
#include "abstractutilityoperation.h"
#include "flipperzero/assetmanifest.h"

class QIODevice;

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

    AssetsDownloadOperation(CommandInterface *cli, DeviceState *deviceState, QIODevice *compressedFile, QObject *parent = nullptr);
    ~AssetsDownloadOperation();

    const QString description() const override;

private slots:
    void advanceOperationState() override;

private:
    bool checkForExtStorage();
    bool extractArchive();
    bool readLocalManifest();
    bool checkForDeviceManifest();
    bool readDeviceManifest();

    bool buildFileLists();

    bool deleteFiles();
    bool writeFiles();

    QIODevice *m_compressedFile;
    QIODevice *m_uncompressedFile;

    TarArchive m_archive;

    AssetManifest m_localManifest;
    AssetManifest m_deviceManifest;
    bool m_isDeviceManifestPresent;

    FileNode::FileInfoList m_deleteList;
    FileNode::FileInfoList m_writeList;
};

}
}

