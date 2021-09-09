#include "assetsdownloadoperation.h"

#include <QFile>
#include <QStandardPaths>

#include "flipperzero/storage/statoperation.h"
#include "flipperzero/storagecontroller.h"
#include "flipperzero/flipperzero.h"
#include "gzipuncompressor.h"
#include "tararchive.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

AssetsDownloadOperation::AssetsDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    Operation(device, parent),
    m_compressed(file),
    m_uncompressed(nullptr)
{}

AssetsDownloadOperation::~AssetsDownloadOperation()
{}

const QString AssetsDownloadOperation::description() const
{
    return QStringLiteral("Assets Download @%1 %2").arg(device()->model(), device()->name());
}

void AssetsDownloadOperation::transitionToNextState()
{
    // Determine if /ext is present
    // Extract archive +
    // Build tar file +
    // Parse manifest -- skipping for now
    // Build file lists
    // Remove files
    // Copy files
    // Finish

    if(state() == BasicState::Ready) {
        setState(State::CheckingExtStorage);
        if(!checkForExtStorage()) {
            finishWithError(QStringLiteral("Failed to access the external storage"));
        }

    } else if(state() == State::CheckingExtStorage) {
        setState(State::ExtractingArchive);
        if(!extractArchive()) {
            finishWithError(QStringLiteral("Failed to extract the databases archive"));
        }

    } else if(state() == State::ExtractingArchive) {
        m_archive = TarArchive(m_uncompressed);

        if(!m_archive.isValid()) {
            finishWithError(QStringLiteral("Failed to parse the databases archive"));
        }

//        const auto files = m_archive.files();
//        for(const auto &fileInfo : files) {
//            qDebug() << fileInfo.name();
//        }
    }

    if(isError()) {
        device()->setError(errorString());
    }
}

void AssetsDownloadOperation::onOperationTimeout()
{
    qDebug() << "Operation timeout";
}

bool AssetsDownloadOperation::checkForExtStorage()
{
    auto *op = device()->storage()->stat("/ext");

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError("Failed to perform stat operation");
        } else if(op->type() == StatOperation::Type::InternalError) {
            info_msg("No external storage found, finishing early");
            finish();

        } else if(op->type() != StatOperation::Type::Storage) {
            finishWithError("/ext is not a storage");
        } else {
            transitionToNextState();
        }

        if(isError()) {
            device()->setError(errorString());
        }

        op->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::extractArchive()
{
    // TODO: put everything into a directory & generate random names for the files
    const auto tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    check_return_bool(!tempPath.isEmpty(), "Failed to find a suitable temporary location.");

    // TODO: check if file exists, etc.
    m_uncompressed = new QFile(tempPath + "/qflipper-databases.tar");

    auto *uncompressor = new GZipUncompressor(m_compressed, m_uncompressed, this);
    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            info_msg("external storage is present.")
            transitionToNextState();
        }

        uncompressor->deleteLater();
        m_compressed->deleteLater();
    });

    return true;
}
