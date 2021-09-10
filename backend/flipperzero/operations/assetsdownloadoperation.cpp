#include "assetsdownloadoperation.h"

#include <QFile>
#include <QTimer>
#include <QStandardPaths>

#include "flipperzero/storage/removeoperation.h"
#include "flipperzero/storage/statoperation.h"
#include "flipperzero/storagecontroller.h"
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
        setState(State::CheckingFiles);

        if(!buildFileList()) {
            finishWithError(QStringLiteral("Failed to build file list"));
        } else if(!checkFiles()) {
            finishWithError(QStringLiteral("Failed to start checking for existing files"));
        }

    } else if(state() == State::CheckingFiles) {
        setState(State::DeletingFiles);

        if(!deleteFiles()) {
            finishWithError(QStringLiteral("Failed to delete files"));
        }

    } else if(state() == State::DeletingFiles) {
        qDebug() << "============= Yay deleted!";
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

    // TODO: check if file exists, etc.
    m_uncompressed = new QFile(tempPath + "/qflipper-databases.tar");

    auto *uncompressor = new GZipUncompressor(m_compressed, m_uncompressed, this);

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            transitionToNextState();
        }

        uncompressor->deleteLater();
        m_compressed->deleteLater();
    });

    return true;
}

bool AssetsDownloadOperation::buildFileList()
{
    m_archive = TarArchive(m_uncompressed);

    if(!m_archive.isValid()) {
        error_msg("Failed to parse the databases archive");
        return false;
    }

    const auto files = m_archive.files();

    for(const auto &fileInfo : files) {
        if(!fileInfo.name().startsWith(RESOURCES_PREFIX) ||
            fileInfo.name() == RESOURCES_PREFIX) {
            continue;
        }

        m_files.append(fileInfo);
    }

    return true;
}

bool AssetsDownloadOperation::checkFiles()
{
    auto i = 0;
    for(const auto &fileInfo : qAsConst(m_files)) {
        const auto fileName = QStringLiteral("/ext") + fileInfo.name().mid(RESOURCES_PREFIX.size());
        const auto isLast = (++i == m_files.size());

        auto *op = device()->storage()->stat(fileName.toLocal8Bit());

        connect(op, &AbstractOperation::finished, this, [=]() {
            op->deleteLater();

            if(op->isError()) {
                // TODO: what to do if something fails?
                qDebug() << "============= ERROR!!!!";
                return;

            } else if(op->type() == StatOperation::Type::File) {
                m_delete.append(fileName);
            }

            if(isLast) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        });
    }

    return true;
}

bool AssetsDownloadOperation::deleteFiles()
{
    auto i = 0;
    for(const auto &fileName : qAsConst(m_delete)) {
        const auto isLast = (++i == m_delete.size());

        auto *op = device()->storage()->remove(fileName.toLocal8Bit());

        connect(op, &AbstractOperation::finished, this, [=]() {
            op->deleteLater();

            if(op->isError()) {
                // TODO: what to do if something fails?
                qDebug() << "============= ERROR!!!!";
                return;

            } else if(isLast) {
                QTimer::singleShot(0, this, &AssetsDownloadOperation::transitionToNextState);
            }
        });
    }

    return true;
}
