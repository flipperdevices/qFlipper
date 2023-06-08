#include "firmwarehelper.h"

#include <QFile>
#include <QDebug>
#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/helper/scriptshelper.h"
#include "flipperzero/helper/radiomanifesthelper.h"

#include "remotefilefetcher.h"
#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

FirmwareHelper::FirmwareHelper(DeviceState *deviceState, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractOperationHelper(parent),
    m_deviceState(deviceState),
    m_versionInfo(versionInfo)
{}

FirmwareHelper::~FirmwareHelper()
{
    for(const auto &file : qAsConst(m_files)) {
        file->remove();
    }

    m_files.clear();
}

QFile *FirmwareHelper::file(FileIndex index) const
{
    return m_files.value(index, nullptr);
}

bool FirmwareHelper::hasRadioUpdate() const
{
    return m_hasRadioUpdate;
}

void FirmwareHelper::nextStateLogic()
{
    if(state() == AbstractOperationHelper::Ready) {
        setState(FirmwareHelper::FetchingFirmware);
        fetchFirmware();

    } else if(state() == FirmwareHelper::FetchingFirmware) {
        setState(FirmwareHelper::FetchingCore2Firmware);
        fetchCore2Firmware();

    } else if(state() == FirmwareHelper::FetchingCore2Firmware) {
        setState(FirmwareHelper::PreparingRadioFirmware);
        prepareRadioFirmware();

    } else if(state() == FirmwareHelper::PreparingRadioFirmware) {
        setState(FirmwareHelper::FetchingScripts);
        fetchScripts();

    } else if(state() == FirmwareHelper::FetchingScripts) {
        setState(FirmwareHelper::PreparingOptionBytes);
        prepareOptionBytes();

    } else if(state() == FirmwareHelper::PreparingOptionBytes) {
        setState(FirmwareHelper::FetchingAssets);
        fetchAssets();

    } else if(state() == FirmwareHelper::FetchingAssets) {
        finish();
    }
}

void FirmwareHelper::fetchFirmware()
{
    m_deviceState->setStatusString(QStringLiteral("Fetching application firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("full_dfu"), m_deviceState->deviceInfo().hardware.target);
    fetchFile(FileIndex::Firmware, fileInfo);
}

void FirmwareHelper::fetchCore2Firmware()
{
    m_deviceState->setStatusString(QStringLiteral("Fetching radio firmware..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("core2_firmware_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::Core2Tgz, fileInfo);
}

void FirmwareHelper::prepareRadioFirmware()
{
    m_deviceState->setStatusString(QStringLiteral("Preparing radio firmware..."));
    auto *helper = new RadioManifestHelper(m_files[FileIndex::Core2Tgz], this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            finishWithError(helper->error(), helper->errorString());
            return;
        }

        const auto &newRadioVersion = helper->radioVersion();
        const auto &newStackType = helper->stackType();

        const auto &currentRadioVersion = m_deviceState->deviceInfo().radioVersion;
        const auto &currentStackType = m_deviceState->deviceInfo().stackType;

        m_hasRadioUpdate = currentRadioVersion.isEmpty() || (currentStackType != newStackType) || (currentRadioVersion < newRadioVersion);

        auto *file = globalTempDirs->createTempFile(this);
        m_files.insert(FileIndex::RadioFirmware, file);

        if(!file->open(QIODevice::WriteOnly)) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to open temporary file: %1").arg(file->errorString()));
            return;
        } else if(file->write(helper->radioFirmwareData()) <= 0) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to write to temporary file: %1").arg(file->errorString()));
            return;
        } else {
            file->close();
        }

        advanceState();
    });
}

void FirmwareHelper::fetchScripts()
{
    m_deviceState->setStatusString(QStringLiteral("Fetching scripts..."));
    const auto &fileInfo = m_versionInfo.fileInfo(QStringLiteral("scripts_tgz"), QStringLiteral("any"));
    fetchFile(FileIndex::ScriptsTgz, fileInfo);
}

void FirmwareHelper::prepareOptionBytes()
{
    m_deviceState->setStatusString(QStringLiteral("Preparing scripts..."));
    auto *helper = new ScriptsHelper(m_files[FileIndex::ScriptsTgz], this);

    connect(helper, &AbstractOperationHelper::finished, this, [=]() {
        helper->deleteLater();

        if(helper->isError()) {
            finishWithError(helper->error(), helper->errorString());
            return;
        }

        auto *file = globalTempDirs->createTempFile(this);
        m_files.insert(FileIndex::OptionBytes, file);

        if(!file->open(QIODevice::WriteOnly)) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to open temporary file: %1").arg(file->errorString()));
        } else if(file->write(helper->optionBytesData()) <= 0) {
            finishWithError(BackendError::DiskError, QStringLiteral("Failed to write to temporary file: %1").arg(file->errorString()));
        } else {
            file->close();
            advanceState();
        }
    });
}

void FirmwareHelper::fetchAssets()
{
    m_deviceState->setStatusString(QStringLiteral("Fetching databases..."));

    const auto type = QStringLiteral("resources_tgz");
    auto fileInfo = m_versionInfo.fileInfo(type, m_deviceState->deviceInfo().hardware.target);

    if(!fileInfo.isValid()) {
        fileInfo = m_versionInfo.fileInfo(type, QStringLiteral("any"));
    }

    fetchFile(FileIndex::AssetsTgz, fileInfo);
}

void FirmwareHelper::fetchFile(FileIndex index, const Updates::FileInfo &fileInfo)
{
    if(!fileInfo.isValid()) {
        finishWithError(BackendError::DataError, QStringLiteral("File info invalid (missing target?)"));
        return;
    }

    const auto fileName = QUrl(fileInfo.url()).fileName();

    auto *file = globalTempDirs->createFile(fileName, this);
    auto *fetcher = new RemoteFileFetcher(fileInfo, file, this);

    if(fetcher->isError()) {
        finishWithError(fetcher->error(), QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        return;
    }

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        m_files.insert(index, file);

        if(fetcher->isError()) {
            finishWithError(fetcher->error(), QStringLiteral("Failed to fetch file: %1").arg(fetcher->errorString()));
        } else {
            advanceState();
        }
    });
}
