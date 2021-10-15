#include "fullrepairoperation.h"

#include <QFile>
#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "flipperzero/helper/scriptshelper.h"
#include "flipperzero/helper/radiomanifesthelper.h"

#include "remotefilefetcher.h"
#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

FullRepairOperation::FullRepairOperation(RecoveryInterface *recovery, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_versionInfo(versionInfo)
{}

FullRepairOperation::~FullRepairOperation()
{
    cleanupFiles();
}

const QString FullRepairOperation::description() const
{
    return QStringLiteral("Full Restore @%1").arg(deviceState()->name());
}

void FullRepairOperation::nextStateLogic()
{
    if(operationState() == FullRepairOperation::Ready) {
        setOperationState(FullRepairOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FullRepairOperation::FetchingFirmware) {
        setOperationState(FullRepairOperation::FetchingCore2Firmware);
        fetchCore2Firmware();

    } else if(operationState() == FullRepairOperation::FetchingCore2Firmware) {
        setOperationState(FullRepairOperation::PreparingRadioFirmware);
        prepareRadioFirmware();

    } else if(operationState() == FullRepairOperation::PreparingRadioFirmware) {
        setOperationState(FullRepairOperation::FetchingScripts);
        fetchScripts();

    } else if(operationState() == FullRepairOperation::FetchingScripts) {
        setOperationState(FullRepairOperation::PreparingOptionBytes);
        prepareOptionBytes();

    } else if(operationState() == FullRepairOperation::PreparingOptionBytes) {
        setOperationState(FullRepairOperation::SettingBootMode);
        setBootMode();

    } else if(operationState() == FullRepairOperation::SettingBootMode) {
        setOperationState(FullRepairOperation::DownloadingRadioFirmware);
        downloadRadioFirmware();

    } else if(operationState() == FullRepairOperation::DownloadingRadioFirmware) {
        setOperationState(FullRepairOperation::DownloadingFirmware);
        downloadFirmware();

    } else if(operationState() == FullRepairOperation::DownloadingFirmware) {
        setOperationState(FullRepairOperation::CorrectingOptionBytes);
        correctOptionBytes();

    } else if(operationState() == FullRepairOperation::CorrectingOptionBytes) {
        setOperationState(FullRepairOperation::CleaningUp);
        cleanupFiles();
        advanceOperationState();

    } else if(operationState() == FullRepairOperation::CleaningUp) {
        finish();
    }
}

void FullRepairOperation::fetchFirmware()
{
    deviceState()->setStatusString(QStringLiteral("Fetching application firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("full_dfu"), deviceState()->deviceInfo().target);
    fetchFile(FileIndex::Firmware, fileInfo);
}

void FullRepairOperation::fetchCore2Firmware()
{
    deviceState()->setStatusString(QStringLiteral("Fetching radio firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("core2_firmware_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::Core2Tgz, fileInfo);
}

void FullRepairOperation::prepareRadioFirmware()
{
    deviceState()->setStatusString(QStringLiteral("Preparing radio firmware..."));
    auto *helper = new RadioManifestHelper(m_files[FileIndex::Core2Tgz], this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            finishWithError(helper->errorString());
            return;
        }

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

        advanceOperationState();
    });
}

void FullRepairOperation::fetchScripts()
{
    deviceState()->setStatusString(QStringLiteral("Fetching scripts..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("scripts_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::ScriptsTgz, fileInfo);
}

void FullRepairOperation::prepareOptionBytes()
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

void FullRepairOperation::setBootMode()
{
    registerOperation(m_recovery->setRecoveryBootMode());
}

void FullRepairOperation::downloadRadioFirmware()
{
    registerOperation(m_recovery->downloadWirelessStack(m_files[FileIndex::RadioFirmware]));
}

void FullRepairOperation::downloadFirmware()
{
    registerOperation(m_recovery->downloadFirmware(m_files[FileIndex::Firmware]));
}

void FullRepairOperation::correctOptionBytes()
{
    registerOperation(m_recovery->fixOptionBytes(m_files[FileIndex::OptionBytes]));
}

void FullRepairOperation::cleanupFiles()
{
    for(const auto &file : qAsConst(m_files)) {
        file->remove();
    }

    m_files.clear();
}

void FullRepairOperation::fetchFile(FileIndex index, const Updates::FileInfo &fileInfo)
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
