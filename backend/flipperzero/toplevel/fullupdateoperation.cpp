#include "fullupdateoperation.h"

#include <QFile>

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
#include "tempdirectories.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

FullUpdateOperation::FullUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_versionInfo(versionInfo),
    m_workDir(tempDirs()->root()),
    m_firmwareFile(nullptr),
    m_assetsFile(nullptr)
{}

const QString FullUpdateOperation::description() const
{
    return QStringLiteral("Full Update @%1").arg(deviceState()->name());
}

void FullUpdateOperation::nextStateLogic()
{
    if(operationState() == FullUpdateOperation::Ready) {
        setOperationState(FullUpdateOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FullUpdateOperation::FetchingFirmware) {
        setOperationState(FullUpdateOperation::FetchingWirelessStack);
        fetchWirelessStack();

    } else if(operationState() == FullUpdateOperation::FetchingWirelessStack) {
        setOperationState(FullUpdateOperation::FetchingScripts);
        fetchScripts();

    } else if(operationState() == FullUpdateOperation::FetchingScripts) {
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

void FullUpdateOperation::onSubOperationErrorOccured()
{
    cleanupFiles();
}

void FullUpdateOperation::fetchFirmware()
{
    deviceState()->setStatusString(QStringLiteral("Fetching firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("full_dfu"), deviceState()->deviceInfo().target);
    m_firmwareFile = fetchFile(fileInfo);
}

void FullUpdateOperation::fetchWirelessStack()
{
    deviceState()->setStatusString(QStringLiteral("Fetching wireless stack..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("core2_firmware_tgz"), QStringLiteral("any"));
    m_wirelessStackFile = fetchFile(fileInfo);
}

void FullUpdateOperation::fetchScripts()
{
    deviceState()->setStatusString(QStringLiteral("Fetching scripts..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("scripts_tgz"), QStringLiteral("any"));
    m_scriptsFile = fetchFile(fileInfo);
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
        if(!file->bytesAvailable()) {
            finishWithError(QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        } else {
            advanceOperationState();
        }

        file->close();
    });

    if(!fetcher->fetch(fileInfo, file)) {
        finishWithError(QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        return nullptr;
    } else {
        return file;
    }
}
