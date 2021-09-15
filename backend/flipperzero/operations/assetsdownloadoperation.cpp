#include "assetsdownloadoperation.h"

#include <QFile>
#include <QTimer>
#include <QStandardPaths>

#include "flipperzero/storage/removeoperation.h"
#include "flipperzero/storage/readoperation.h"
#include "flipperzero/storage/statoperation.h"
#include "flipperzero/storagecontroller.h"
#include "flipperzero/assetmanifest.h"
#include "flipperzero/flipperzero.h"
#include "gzipuncompressor.h"
#include "tararchive.h"

#include "macros.h"

#define RESOURCES_PREFIX QByteArrayLiteral("resources")

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
        setState(UploadingManifest);
        if(!readManifest()) {
            finishWithError(QStringLiteral("Failed to upload manifest file"));
        }

    } else if(state() == State::UploadingManifest) {
        setState(State::CheckingFiles);
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
    auto *op = device()->storage()->stat(QByteArrayLiteral("/ext"));

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError("Failed to perform stat operation");
        } else if(op->type() == StatOperation::Type::InternalError) {
            info_msg("No external storage found, finishing early");
            finish();

        } else if(op->type() != StatOperation::Type::Storage) {
            finishWithError("/ext is not a storage");
        } else {
            info_msg(QStringLiteral("External storage is present, %1 bytes free.").arg(op->sizeFree()));
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

    check_return_bool(m_compressed->open(QIODevice::ReadOnly), m_compressed->errorString());

    // TODO: check if file exists, etc.
    m_uncompressed = new QFile(tempPath + "/qflipper-databases.tar", this);
    check_return_bool(m_uncompressed->open(QIODevice::ReadWrite), m_uncompressed->errorString());

    auto *uncompressor = new GZipUncompressor(m_compressed, m_uncompressed, this);

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            m_archive = TarArchive(m_uncompressed);

            if(m_archive.isError()) {
                finishWithError(m_archive.errorString());
            } else {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        }

        uncompressor->deleteLater();
        m_compressed->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::readManifest()
{
    auto *op = device()->storage()->read(QByteArrayLiteral("/ext/Manifest"));

    connect(op, &AbstractOperation::finished, this, [=]() {
        const auto success = buildFileLists(op->isError() ?  QByteArray() : op->result());

        if(!success) {
            finishWithError(QStringLiteral("Failed to build file lists"));
        } else {
            qDebug() << " ====================== All clear!";
        }

        op->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::buildFileLists(const QByteArray &manifestText)
{
    const auto here = AssetManifest(m_archive.fileData(QStringLiteral("resources/Manifest")));
    check_return_bool(!here.isError(), QStringLiteral("Failed to build local manifest: %1").arg(here.errorString()));

    qDebug() << "Here:";
    here.print();

    const auto there = AssetManifest(manifestText);

    if(there.isError()) {
        info_msg("Failed to build remote manifest, assuming complete asset overwrite...");
        // Put all files from here to m_delete and m_write
        return true;
    }

    qDebug() << "There:";
    there.print();

    // Calculate the difference between trees

    // Put files to delete and to write in m_delete

    // Put files to write to m_write

    return true;
}

bool AssetsDownloadOperation::deleteFiles()
{
    return true;
}

bool AssetsDownloadOperation::writeFiles()
{
    return true;
}
