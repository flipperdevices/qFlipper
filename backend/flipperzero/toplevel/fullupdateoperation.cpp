#include "fullupdateoperation.h"

#include <QFile>
#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/userbackupoperation.h"
#include "flipperzero/utility/userrestoreoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "flipperzero/helper/scriptshelper.h"
#include "flipperzero/helper/radiomanifesthelper.h"

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
    m_updateRadio(false)
{}

FullUpdateOperation::~FullUpdateOperation()
{
    cleanupFiles();
}

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
        setOperationState(FullUpdateOperation::FetchingCore2Firmware);
        fetchCore2Firmware();

    } else if(operationState() == FullUpdateOperation::FetchingCore2Firmware) {
        setOperationState(FullUpdateOperation::PreparingRadioFirmware);
        prepareRadioFirmware();

    } else if(operationState() == FullUpdateOperation::PreparingRadioFirmware) {
        setOperationState(FullUpdateOperation::FetchingScripts);
        fetchScripts();

    } else if(operationState() == FullUpdateOperation::FetchingScripts) {
        setOperationState(FullUpdateOperation::PreparingOptionBytes);
        prepareOptionBytes();

    } else if(operationState() == FullUpdateOperation::PreparingOptionBytes) {
        setOperationState(FullUpdateOperation::FetchingAssets);
        fetchAssets();

    } else if(operationState() == FullUpdateOperation::FetchingAssets) {
        setOperationState(FullUpdateOperation::SavingBackup);
        saveBackup();

    } else if(operationState() == FullUpdateOperation::SavingBackup) {
        setOperationState(FullUpdateOperation::StartingRecovery);
        startRecovery();

    } else if(operationState() == FullUpdateOperation::StartingRecovery) {

        if(m_updateRadio) {
            setOperationState(FullUpdateOperation::SettingBootMode);
            setBootMode();

        } else{
            setOperationState(FullUpdateOperation::DownloadingRadioFirmware);
            advanceOperationState();
        }

    } else if(operationState() == FullUpdateOperation::SettingBootMode) {
        setOperationState(FullUpdateOperation::DownloadingRadioFirmware);
        downloadRadioFirmware();

    } else if(operationState() == FullUpdateOperation::DownloadingRadioFirmware) {
        setOperationState(FullUpdateOperation::DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == FullUpdateOperation::DownloadingFirmware) {

        if(m_updateRadio) {
            setOperationState(FullUpdateOperation::CorrectingOptionBytes);
            correctOptionBytes();

        } else {
            setOperationState(FullUpdateOperation::ExitingRecovery);
            exitRecovery();
        }

    } else if((operationState() == FullUpdateOperation::ExitingRecovery) ||
              (operationState() == FullUpdateOperation::CorrectingOptionBytes)) {
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
        advanceOperationState();

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
    deviceState()->setStatusString(QStringLiteral("Fetching application firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("full_dfu"), deviceState()->deviceInfo().target);
    fetchFile(FileIndex::Firmware, fileInfo);
}

void FullUpdateOperation::fetchCore2Firmware()
{
    deviceState()->setStatusString(QStringLiteral("Fetching radio firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("core2_firmware_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::Core2Tgz, fileInfo);
}

void FullUpdateOperation::fetchScripts()
{
    deviceState()->setStatusString(QStringLiteral("Fetching scripts..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("scripts_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::ScriptsTgz, fileInfo);
}

void FullUpdateOperation::fetchAssets()
{
    deviceState()->setStatusString(QStringLiteral("Fetching databases..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("resources_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::AssetsTgz, fileInfo);
}

void FullUpdateOperation::saveBackup()
{
    registerOperation(m_utility->backupInternalStorage(tempDirs()->root().absolutePath()));
}

void FullUpdateOperation::startRecovery()
{
    registerOperation(m_utility->startRecoveryMode());
}

void FullUpdateOperation::prepareRadioFirmware()
{
    deviceState()->setStatusString(QStringLiteral("Preparing radio firmware..."));
    auto *helper = new RadioManifestHelper(m_files[FileIndex::Core2Tgz], this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            finishWithError(helper->errorString());
            return;
        }

        const auto &newRadioVersion = helper->radioVersion();
        const auto &currentRadioVersion = deviceState()->deviceInfo().radioVersion;

        m_updateRadio = currentRadioVersion <= newRadioVersion;

        if(m_updateRadio) {
            auto *file = tempDirs()->createTempFile();
            m_files.insert(FileIndex::RadioFirmware, file);

            if(!file->open(QIODevice::WriteOnly)) {
                finishWithError(QStringLiteral("Failed to open temporary file: %1").arg(file->errorString()));
                return;
            } else if(file->write(helper->radioFirmwareData()) <= 0) {
                finishWithError(QStringLiteral("Failed to write to temporary file: %1").arg(file->errorString()));
                return;
            } else {
                file->close();
            }
        }

        advanceOperationState();
    });
}

void FullUpdateOperation::setBootMode()
{
    registerOperation(m_recovery->setRecoveryBootMode());
}

void FullUpdateOperation::downloadRadioFirmware()
{
    registerOperation(m_recovery->downloadWirelessStack(m_files[FileIndex::RadioFirmware]));
}

void FullUpdateOperation::downloadFirmware()
{
    registerOperation(m_recovery->downloadFirmware(m_files[FileIndex::Firmware]));
}

void FullUpdateOperation::prepareOptionBytes()
{
    deviceState()->setStatusString(QStringLiteral("Preparing scripts..."));
    auto *helper = new ScriptsHelper(m_files[FileIndex::ScriptsTgz], this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            finishWithError(helper->errorString());
            return;
        }

        auto *file = tempDirs()->createTempFile();
        m_files.insert(FileIndex::OptionBytes, file);

        if(!file->open(QIODevice::WriteOnly)) {
            finishWithError(QStringLiteral("Failed to open temporary file: %1").arg(file->errorString()));
        } else if(file->write(helper->optionBytesData()) <= 0) {
            finishWithError(QStringLiteral("Failed to write to temporary file: %1").arg(file->errorString()));
        } else {
            file->close();
            advanceOperationState();
        }
    });
}

void FullUpdateOperation::correctOptionBytes()
{
    registerOperation(m_recovery->fixOptionBytes(m_files[FileIndex::OptionBytes]));
}

void FullUpdateOperation::exitRecovery()
{
    registerOperation(m_recovery->exitRecoveryMode());
}

void FullUpdateOperation::downloadAssets()
{
    registerOperation(m_utility->downloadAssets(m_files[FileIndex::AssetsTgz]));
}

void FullUpdateOperation::restoreBackup()
{
    registerOperation(m_utility->restoreInternalStorage(tempDirs()->root().absolutePath()));
}

void FullUpdateOperation::restartDevice()
{
    registerOperation(m_utility->restartDevice());
}

void FullUpdateOperation::cleanupFiles()
{
    for(const auto &file : qAsConst(m_files)) {
        file->remove();
    }

    m_files.clear();
}

void FullUpdateOperation::fetchFile(FileIndex index, const Updates::FileInfo &fileInfo)
{
    const auto fileName = QUrl(fileInfo.url()).fileName();

    auto *file = tempDirs()->createFile(fileName, this);
    auto *fetcher = new RemoteFileFetcher(fileInfo, file, this);

    if(fetcher->isError()) {
        finishWithError(QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        return;
    }

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        m_files.insert(index, file);

        if(fetcher->isError()) {
            finishWithError(QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        } else {
            advanceOperationState();
        }
    });
}
