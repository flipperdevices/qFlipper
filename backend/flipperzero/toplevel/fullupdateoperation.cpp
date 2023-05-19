#include "fullupdateoperation.h"

#include <QDebug>
#include <QDirIterator>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"
#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/checksumverifyoperation.h"
#include "flipperzero/utility/filesuploadoperation.h"
#include "flipperzero/utility/pathcreateoperation.h"
#include "flipperzero/utility/startupdateroperation.h"
#include "flipperzero/utility/storageinforefreshoperation.h"
#include "flipperzero/utility/regionprovisioningoperation.h"

#include "tarzipuncompressor.h"
#include "tempdirectories.h"
#include "remotefilefetcher.h"

#define REMOTE_DIR "/ext/update"

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

static inline const QString getBaseName(const QString &url)
{
    const auto start = url.lastIndexOf('/') + 1;
    const auto end = url.lastIndexOf('.');
    return url.mid(start, end - start);
}

using namespace Flipper;
using namespace Zero;

FullUpdateOperation::FullUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_updateFile(nullptr),
    m_utility(utility),
    m_versionInfo(versionInfo)
{}

FullUpdateOperation::FullUpdateOperation(UtilityInterface *utility, DeviceState *deviceState, const QUrl &bundleUrl, QObject *parent):
    AbstractTopLevelOperation(deviceState, parent),
    m_updateFile(new QFile(bundleUrl.toLocalFile(), this)),
    m_utility(utility)
{}

FullUpdateOperation::~FullUpdateOperation()
{
    deviceState()->setAllowVirtualDisplay(true);
}

const QString FullUpdateOperation::description() const
{
    return QStringLiteral("Full Update @%1").arg(deviceState()->name());
}

void FullUpdateOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(ProvisioninigRegion);
        provisionRegionData();

    } else if(operationState() == ProvisioninigRegion) {
        setOperationState(CheckingStorage);
        checkStorage();

    } else if(operationState() == CheckingStorage) {
        if(!m_updateFile) {
            setOperationState(FetchingUpdate);
            fetchUpdateFile();
        } else {
            setOperationState(PreparingLocalUpdate);
            prepareLocalUpdate();
        }

    } else if(operationState() == FetchingUpdate ||
              operationState() == PreparingLocalUpdate) {
        setOperationState(ExtractingUpdate);
        extractUpdate();

    } else if(operationState() == ExtractingUpdate) {
        setOperationState(ReadingUpdateFiles);
        readUpdateFiles();

    } else if(operationState() == ReadingUpdateFiles) {
        setOperationState(PreparingRemoteUpdate);
        createUpdatePath();

    } else if(operationState() == PreparingRemoteUpdate) {
        setOperationState(VerifyingExistingFiles);
        verifyExistingFiles();

    } else if(operationState() == VerifyingExistingFiles) {
        setOperationState(UploadingUpdateFiles);
        uploadUpdateFiles();

    } else if(operationState() == UploadingUpdateFiles) {
        setOperationState(WaitingForUpdate);
        startUpdate();

    } else if(operationState() == WaitingForUpdate) {
        finish();
    }
}

void FullUpdateOperation::provisionRegionData()
{
    auto *operation = m_utility->provisionRegionData();
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCInfo(CATEGORY_DEBUG) << "Warning: failed to perform region data provisioning:" << operation->errorString();
        }

        advanceOperationState();
    });
}

void FullUpdateOperation::checkStorage()
{
    deviceState()->setStatusString(QStringLiteral("Checking storage..."));

    auto *operation = m_utility->refreshStorageInfo();
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::OperationError, QStringLiteral("Failed to check device storage"));
        } else if(!deviceState()->deviceInfo().storage.isExternalPresent) {
            finishWithError(BackendError::UnknownError, "SD Card is not installed or malfunctioning");
        } else {
            advanceOperationState();
        }
    });
}

void FullUpdateOperation::fetchUpdateFile()
{
    deviceState()->setStatusString(QStringLiteral("Fetching firmware update..."));

    const auto target = deviceState()->deviceInfo().hardware.target;
    const auto fileInfo = m_versionInfo.fileInfo(QStringLiteral("update_tgz"), target);

    if(fileInfo.target() != target) {
        finishWithError(BackendError::DataError, QStringLiteral("Required file type or target not found"));
        return;
    }

    m_updateFile = globalTempDirs->createTempFile(this);
    m_updateDirectory = globalTempDirs->subdir(getBaseName(fileInfo.url()));

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

void FullUpdateOperation::prepareLocalUpdate()
{
    deviceState()->setStatusString(QStringLiteral("Preparing local firmware update..."));
    m_updateDirectory = globalTempDirs->subdir(getBaseName(m_updateFile->fileName()));
    advanceOperationState();
}

void FullUpdateOperation::extractUpdate()
{
    deviceState()->setStatusString(QStringLiteral("Extracting firmware update ..."));
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

void FullUpdateOperation::readUpdateFiles()
{
    deviceState()->setStatusString(QStringLiteral("Reading firmware update ..."));
    deviceState()->setProgress(-1.0);

    const auto subdirNames = m_updateDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if(subdirNames.isEmpty()) {
        finishWithError(BackendError::DataError, QStringLiteral("Cannot find update directory"));
        return;

    } else if(!m_updateDirectory.cd(subdirNames.first())) {
        finishWithError(BackendError::DataError, QStringLiteral("Cannot enter update directory"));
        return;
    }

    const auto fileNames = m_updateDirectory.entryList(QDir::Files);
    for(const auto &fileName : fileNames) {
        m_fileUrls.append(QUrl::fromLocalFile(m_updateDirectory.absoluteFilePath(fileName)));
    }

    advanceOperationState();
}

void FullUpdateOperation::createUpdatePath()
{
    deviceState()->setStatusString(QStringLiteral("Creating update path ..."));
    deviceState()->setProgress(-1.0);

    const auto remotePath = QStringLiteral("%1/%2").arg(REMOTE_DIR, m_updateDirectory.dirName()).toLocal8Bit();
    auto *operation = m_utility->createPath(remotePath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;

        } else if(!operation->pathExists()) {
            setOperationState(VerifyingExistingFiles);
        }

        advanceOperationState();
    });
}

void FullUpdateOperation::verifyExistingFiles()
{
    deviceState()->setStatusString(QStringLiteral("Verifying existing files ..."));
    deviceState()->setProgress(-1.0);

    const auto remotePath = QStringLiteral("%1/%2").arg(REMOTE_DIR, m_updateDirectory.dirName()).toLocal8Bit();
    auto *operation = m_utility->verifyChecksum(m_fileUrls, remotePath);

    connect(operation, &AbstractOperation::progressChanged, this, [=]() {
        deviceState()->setProgress(operation->progress());
    });

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;
        }

        m_fileUrls = operation->changedUrls();

        if(m_fileUrls.isEmpty()) {
            qCDebug(CATEGORY_DEBUG) << "Update package has been already uploaded, skipping to update...";
            setOperationState(FullUpdateOperation::UploadingUpdateFiles);
        }

        advanceOperationState();
    });
}

void FullUpdateOperation::uploadUpdateFiles()
{
    deviceState()->setStatusString(QStringLiteral("Uploading firmware update ..."));

    const auto remotePath = QStringLiteral("%1/%2").arg(QStringLiteral(REMOTE_DIR), m_updateDirectory.dirName()).toLocal8Bit();
    auto *operation = m_utility->uploadFiles(m_fileUrls, remotePath);

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

void FullUpdateOperation::startUpdate()
{
    deviceState()->setAllowVirtualDisplay(false);

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
