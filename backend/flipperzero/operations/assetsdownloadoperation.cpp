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

static void print_file_list(const QString &header, const QStringList &list)
{
    qDebug() << header;

    for(const auto &e : list) {
        qDebug() << e;
    }
}

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
        setState(ReadingLocalManifest);
        if(!readLocalManifest()) {
            finishWithError(QStringLiteral("Failed to read local manifest"));
        }

    } else if(state() == State::ReadingLocalManifest) {
        setState(ReadingDeviceManifest);
        if(!readDeviceManifest()) {
            finishWithError(QStringLiteral("Failed to read device manifest"));
        }

    } else if(state() == State::ReadingDeviceManifest) {
        setState(State::BuildingFileLists);
        if(!buildFileLists()) {
            finishWithError(QStringLiteral("Failed to build file lists"));
        }

    } else if(state() == State::BuildingFileLists) {
        setState(State::DeletingFiles);
        qDebug() << "==================== YAY! =================";
//        if(!deleteFiles()) {
//            finishWithError(QStringLiteral("Failed to delete files"));
//        }

    } else if(state() == State::DeletingFiles) {
        setState(State::WritingFiles);

    } else if(state() == State::WritingFiles) {
        finish();
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
            info_msg("No external storage found, finishing early.");
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

bool AssetsDownloadOperation::readLocalManifest()
{
    const auto text = m_archive.fileData(QStringLiteral("resources/Manifest"));
    check_return_bool(!text.isEmpty(), m_archive.errorString());

    m_localManifest = AssetManifest(text);
    check_return_bool(!m_localManifest.isError(), m_localManifest.errorString());

    QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    return true;
}

bool AssetsDownloadOperation::readDeviceManifest()
{
    auto *op = device()->storage()->read(QByteArrayLiteral("/ext/Manifest"));

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(!op->isError()) {
            m_deviceManifest = AssetManifest(op->result());
        }

        op->deleteLater();
        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    });

    return true;
}

bool AssetsDownloadOperation::buildFileLists()
{
    info_msg("<<<<< Local manifest:");
    m_localManifest.tree()->print();

    m_delete.append(QStringLiteral("Manifest"));
    m_write.append(QStringLiteral("Manifest"));

    if(m_deviceManifest.isError()) {
        info_msg("Failed to build remote manifest, assuming complete asset overwrite...");
        m_write.append(m_localManifest.tree()->toPreOrderList().mid(1));
        return true;
    }

    info_msg(">>>>> Device manifest:");
    m_deviceManifest.tree()->print();

    const auto deleted = m_localManifest.tree()->difference(m_deviceManifest.tree());
    print_file_list("----- Files deleted:", deleted);

    const auto added = m_deviceManifest.tree()->difference(m_localManifest.tree());
    print_file_list("+++++ Files added:", added);

    const auto changed = m_deviceManifest.tree()->changed(m_localManifest.tree());
    print_file_list("***** Files changed:", changed);

    m_delete.append(deleted);
    m_delete.append(changed);

    m_write.append(changed);
    m_write.append(added);

    std::sort(m_delete.rbegin(), m_delete.rend());
    std::sort(m_write.rbegin(), m_write.rend());

    print_file_list("##### Final list for deletion:", m_delete);
    print_file_list("##### Final list for writing:", m_write);

    QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
    return true;
}

bool AssetsDownloadOperation::deleteFiles()
{
    if(m_delete.isEmpty()) {
        info_msg("No files to delete, skipping to write");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
        return true;
    }

    int i = m_delete.size();

    for(const auto &filePath : qAsConst(m_delete)) {
        --i;
        auto *op = device()->storage()->remove(QByteArrayLiteral("/ext/") + filePath.toLocal8Bit());
        connect(op, &AbstractOperation::finished, this, [=]() {
            if(op->isError()) {
                finishWithError(op->errorString());
            } else if(i == 0) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        });
    }

    return true;
}

bool AssetsDownloadOperation::writeFiles()
{
    if(m_write.isEmpty()) {
        info_msg("No files to write, skipping to the end");
        QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
        return true;
    }

//    int i = m_write.size();

//    for(const auto &filePath : qAsConst(m_delete)) {
//        --i;
//        auto *op = device()->storage()->remove(QByteArrayLiteral("/ext/") + filePath.toLocal8Bit());
//        connect(op, &AbstractOperation::finished, this, [=]() {
//            if(op->isError()) {
//                finishWithError(op->errorString());
//            } else if(i == 0) {
//                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
//            }
//        });
//    }

    return true;
}
