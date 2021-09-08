#pragma once

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
        ExtractingArchive,
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

    QIODevice *m_compressed;
    QIODevice *m_uncompressed;

    TarArchive m_archive;
};

}
}

