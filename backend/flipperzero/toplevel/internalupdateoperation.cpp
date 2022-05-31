#include "internalupdateoperation.h"

#include <QDebug>
#include <QDirIterator>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"
#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/updateprepareoperation.h"
#include "flipperzero/utility/directoryuploadoperation.h"
#include "flipperzero/utility/startupdateroperation.h"

#include "tarzipuncompressor.h"
#include "tempdirectories.h"
#include "remotefilefetcher.h"

#define REMOTE_DIR "/ext/update"

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

using namespace Flipper;
using namespace Zero;

InternalUpdateOperation::InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    Flipper::Zero::AbstractTopLevelOperation(state, parent),
    m_updateFile(nullptr),
    m_utility(utility),
    m_versionInfo(versionInfo)
{}

InternalUpdateOperation::~InternalUpdateOperation()
{
    deviceState()->setAllowVirtualDisplay(true);
}

const QString InternalUpdateOperation::description() const
{
    return QStringLiteral("Internal Update @%1").arg(deviceState()->name());
}

void InternalUpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(InternalUpdateOperation::FetchingUpdate);
        fetchUpdateFile();

    } else if(operationState() == InternalUpdateOperation::FetchingUpdate) {
        setOperationState(InternalUpdateOperation::ExtractingUpdate);
        extractUpdate();

    } else if(operationState() == InternalUpdateOperation::ExtractingUpdate) {
        setOperationState(InternalUpdateOperation::PreparingUpdateDir);
        prepareUpdateDir();

    } else if(operationState() == InternalUpdateOperation::PreparingUpdateDir) {
        setOperationState(InternalUpdateOperation::UploadingUpdateDir);
        uploadUpdateDir();

    } else if(operationState() == InternalUpdateOperation::UploadingUpdateDir) {
        setOperationState(InternalUpdateOperation::WaitingForUpdate);
        startUpdate();

    } else if(operationState() == InternalUpdateOperation::WaitingForUpdate) {
        finish();
    }
}

void InternalUpdateOperation::fetchUpdateFile()
{
    deviceState()->setStatusString(QStringLiteral("Fetchig fimware update..."));

    const auto target = deviceState()->deviceInfo().hardware.target;
    const auto fileInfo = m_versionInfo.fileInfo(QStringLiteral("update_tgz"), target);

    if(fileInfo.target() != target) {
        finishWithError(BackendError::DataError, QStringLiteral("Required file type or target not found"));
        return;
    }

    m_updateFile = globalTempDirs->createTempFile(this);
    m_updateDirectory = globalTempDirs->subdir(QFileInfo(fileInfo.url()).baseName());

    auto *fetcher = new RemoteFileFetcher(this);
    if(!fetcher->fetch(fileInfo, m_updateFile)) {
        finishWithError(fetcher->error(), fetcher->errorString());
        return;
    }

    connect(fetcher, &RemoteFileFetcher::progressChanged, this, [=](double progress) {
        deviceState()->setProgress(progress);
    });

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(fetcher->isError()) {
            finishWithError(fetcher->error(), fetcher->errorString());
        } else {
            advanceOperationState();
        }

        fetcher->deleteLater();
    });
}

void InternalUpdateOperation::extractUpdate()
{
    deviceState()->setStatusString(QStringLiteral("Extracting fimware update ..."));
    deviceState()->setProgress(-1.0);

    auto *uncompressor = new TarZipUncompressor(m_updateFile, m_updateDirectory, this);

    connect(uncompressor, &TarZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->error(), uncompressor->errorString());
        } else {
            advanceOperationState();
        }

        uncompressor->deleteLater();
    });
}

void InternalUpdateOperation::prepareUpdateDir()
{
    deviceState()->setStatusString(QStringLiteral("Preparing fimware update ..."));
    deviceState()->setProgress(-1.0);

    if(!findAndCdToUpdateDir()) {
        finishWithError(BackendError::DataError, QStringLiteral("Cannot find update directory"));
        return;
    }

    auto *operation = m_utility->prepareUpdateDirectory(m_updateDirectory.dirName().toLocal8Bit(), QByteArrayLiteral(REMOTE_DIR));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;

        } else if(operation->updateDirectoryExists()) {
            qCDebug(CATEGORY_DEBUG) << "Update package has been already uploaded, skipping to update...";
            setOperationState(InternalUpdateOperation::UploadingUpdateDir);
        }

        advanceOperationState();
    });
}

void InternalUpdateOperation::uploadUpdateDir()
{
    deviceState()->setStatusString(QStringLiteral("Uploading fimware update ..."));

    auto *operation = m_utility->uploadDirectory(m_updateDirectory.absolutePath(), QByteArrayLiteral(REMOTE_DIR));

    connect(operation, &AbstractOperation::progressChanged, this, [=]() {
        deviceState()->setProgress(operation->progress());
    });

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            advanceOperationState();
        }
    });
}

void InternalUpdateOperation::startUpdate()
{
    deviceState()->setAllowVirtualDisplay(false);
    deviceState()->setStatusString(QStringLiteral("Uploading fimware update ..."));

    const auto manifestPath = QStringLiteral("%1/%2/update.fuf").arg(QStringLiteral(REMOTE_DIR), m_updateDirectory.dirName());
    auto *operation = m_utility->startUpdater(manifestPath.toLocal8Bit());

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            advanceOperationState();
        }
    });
}

bool InternalUpdateOperation::findAndCdToUpdateDir()
{
    m_updateDirectory.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    QDirIterator it(m_updateDirectory);

    while(it.hasNext()) {
        it.next();

        const auto &fileInfo = it.fileInfo();
        const auto baseName = fileInfo.baseName();

        if(fileInfo.isDir() && m_updateDirectory.dirName().endsWith(baseName)) {
            m_updateDirectory.cd(baseName);
            return true;
        }
    }

    return false;
}
