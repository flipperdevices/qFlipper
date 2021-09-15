#pragma once

#include <QQueue>

#include "tararchive.h"
#include "flipperzerooperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class AssetsDownloadOperation : public Operation
{
    Q_OBJECT

public:
    enum State {
        CheckingExtStorage = BasicState::User,
        UploadingManifest,
        ExtractingArchive,
        CheckingFiles,
        DeletingFiles,
        DownloadingFiles
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
    bool readManifest();

    bool buildFileLists(const QByteArray &manifestText);

    bool deleteFiles();
    bool writeFiles();

    QIODevice *m_compressed;
    QIODevice *m_uncompressed;

    TarArchive m_archive;

    QStringList m_delete;
    QStringList m_write;
};

}
}

