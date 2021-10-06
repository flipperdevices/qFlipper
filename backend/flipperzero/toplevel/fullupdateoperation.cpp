#include "fullupdateoperation.h"

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QStandardPaths>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"

#include "remotefilefetcher.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

FullUpdateOperation::FullUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_versionInfo(versionInfo),
    m_workDir(QDir::temp()),
    m_firmwareFile(nullptr),
    m_assetsFile(nullptr)
{
    deviceState()->setPersistent(true);
    deviceState()->setStatusString(QStringLiteral("Firmware update pending..."));
}

FullUpdateOperation::~FullUpdateOperation()
{
    deviceState()->setPersistent(false);
}

const QString FullUpdateOperation::description() const
{
    const auto &model = deviceState()->deviceInfo().model;
    const auto &name = deviceState()->deviceInfo().name;

    return QStringLiteral("Full Update @%1 %2").arg(model, name);
}

void FullUpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(FullUpdateOperation::CreatingWorkDir);
        createWorkDir();

    } else if(operationState() == FullUpdateOperation::CreatingWorkDir) {
        setOperationState(FullUpdateOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FullUpdateOperation::FetchingFirmware) {
        setOperationState(FullUpdateOperation::FetchingAssets);
        fetchAssets();

    } else if(operationState() == FullUpdateOperation::FetchingAssets) {
        setOperationState(FullUpdateOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == FullUpdateOperation::SavingBackup) {
        setOperationState(FullUpdateOperation::StartingRecovery);
        startRecovery();

    } else if(operationState() == FullUpdateOperation::StartingRecovery) {
        setOperationState(FullUpdateOperation::DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == FullUpdateOperation::DownloadingFirmware) {
        setOperationState(FullUpdateOperation::ExitingRecovery);
        exitRecovery();

    } else if(operationState() == FullUpdateOperation::ExitingRecovery) {
        setOperationState(FullUpdateOperation::DownloadingAssets);
        downloadAssets();

    } else if(operationState() == FullUpdateOperation::DownloadingAssets) {
        setOperationState(FullUpdateOperation::RestoringBackup);
        restoreBackup();

    } else if(operationState() == FullUpdateOperation::RestoringBackup) {
        setOperationState(FullUpdateOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == FullUpdateOperation::RestartingDevice) {
        setOperationState(FullUpdateOperation::CleaningUp);
        cleanupFiles();

    } else if(operationState() == FullUpdateOperation::CleaningUp) {
        finish();
    }
}

void FullUpdateOperation::createWorkDir()
{
    // TODO: add random suffix
    const auto subdir = QStringLiteral("qFlipper");

    bool success = true;

    if(!m_workDir.exists(subdir)) {
        success = m_workDir.mkdir(subdir) && m_workDir.cd(subdir);
    } else {
        success = m_workDir.cd(subdir);
    }

    if(!success) {
        finishWithError(QStringLiteral("Failed to create working directory"));
    } else {
        advanceOperationState();
    }
}

void FullUpdateOperation::fetchFirmware()
{
    deviceState()->setStatusString(QStringLiteral("Fetching firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("full_dfu"), deviceState()->deviceInfo().target);
    m_firmwareFile = fetchFile(fileInfo);
}

void FullUpdateOperation::fetchAssets()
{
    deviceState()->setStatusString(QStringLiteral("Fetching databases..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("resources_tgz"), QStringLiteral("any"));
    m_assetsFile = fetchFile(fileInfo);
}

void FullUpdateOperation::saveBackup()
{
    registerOperation(m_utility->backupInternalStorage(m_workDir.absolutePath()));
}

void FullUpdateOperation::startRecovery()
{
    registerOperation(m_utility->startRecoveryMode());
}

void FullUpdateOperation::downloadFirmware()
{
    registerOperation(m_recovery->downloadFirmware(m_firmwareFile));
}

void FullUpdateOperation::exitRecovery()
{
    registerOperation(m_recovery->exitRecoveryMode());
}

void FullUpdateOperation::downloadAssets()
{
    registerOperation(m_utility->downloadAssets(m_assetsFile));
}

void FullUpdateOperation::restoreBackup()
{
    registerOperation(m_utility->restoreInternalStorage(m_workDir.absolutePath()));
}

void FullUpdateOperation::restartDevice()
{
    registerOperation(m_utility->restartDevice());
}

void FullUpdateOperation::cleanupFiles()
{
    m_workDir.removeRecursively();
    advanceOperationState();
}

QFile *FullUpdateOperation::fetchFile(const Updates::FileInfo &fileInfo)
{
    const auto fileName = QUrl(fileInfo.url()).fileName();
    const auto filePath = m_workDir.absoluteFilePath(fileName);

    auto *file = new QFile(filePath, this);
    auto *fetcher = new RemoteFileFetcher(this);

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        // TODO: make RemoteFileFetcher a Failable
        if(!file->bytesAvailable()) {
            finishWithError(QStringLiteral("Failed to fetch the file (2)"));
        } else {
            advanceOperationState();
        }

        file->close();
        fetcher->deleteLater();
    });

    if(!fetcher->fetch(fileInfo, file)) {
        finishWithError(QStringLiteral("Failed to fetch the file (1)"));
        fetcher->deleteLater();
        file->deleteLater();
        return nullptr;
    }

    return file;
}

void FullUpdateOperation::registerOperation(AbstractOperation *operation)
{
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->errorString());
            cleanupFiles();
        } else {
            advanceOperationState();
        }
    });
}
