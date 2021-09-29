#pragma once

#include <QQueue>

#include "tararchive.h"
#include "flipperzero/flipperzerooperation.h"
#include "flipperzero/assetmanifest.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class AssetsDownloadOperation : public FlipperZeroOperation
{
    Q_OBJECT

public:
    enum State {
        CheckingExtStorage = BasicState::User,
        ExtractingArchive,
        ReadingLocalManifest,
        CheckingDeviceManifest,
        ReadingDeviceManifest,
        BuildingFileLists,
        DeletingFiles,
        WritingFiles
    };

    AssetsDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~AssetsDownloadOperation();

    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    bool checkForExtStorage();
    bool extractArchive();
    bool readLocalManifest();
    bool checkForDeviceManifest();
    bool readDeviceManifest();

    bool buildFileLists();

    bool deleteFiles();
    bool writeFiles();

    QIODevice *m_compressed;
    QIODevice *m_uncompressed;

    TarArchive m_archive;

    AssetManifest m_localManifest;
    AssetManifest m_deviceManifest;
    bool m_isDeviceManifestPresent;

    FileNode::FileInfoList m_delete;
    FileNode::FileInfoList m_write;
};

}
}

