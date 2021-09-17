#pragma once

#include <QQueue>

#include "tararchive.h"
#include "flipperzerooperation.h"
#include "flipperzero/assetmanifest.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class AssetsDownloadOperation : public Operation
{
    Q_OBJECT

public:
    enum State {
        CheckingExtStorage = BasicState::User,
        ExtractingArchive,
        ReadingLocalManifest,
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
    bool readDeviceManifest();

    bool buildFileLists();

    bool deleteFiles();
    bool writeFiles();

    QIODevice *m_compressed;
    QIODevice *m_uncompressed;

    TarArchive m_archive;

    AssetManifest m_localManifest;
    AssetManifest m_deviceManifest;

    FileNode::FileInfoList m_delete;
    FileNode::FileInfoList m_write;
};

}
}

