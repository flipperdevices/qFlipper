#include "internalupdateoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/utilityinterface.h"

#include "tarzipuncompressor.h"
#include "tempdirectories.h"
#include "remotefilefetcher.h"

using namespace Flipper;
using namespace Zero;

InternalUpdateOperation::InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    Flipper::Zero::AbstractTopLevelOperation(state, parent),
    m_firmwareFile(nullptr),
    m_utility(utility),
    m_versionInfo(versionInfo)
{}

const QString InternalUpdateOperation::description() const
{
    return QStringLiteral("Internal Update @%1").arg(deviceState()->name());
}

void InternalUpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(InternalUpdateOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == InternalUpdateOperation::FetchingFirmware) {
        setOperationState(InternalUpdateOperation::ExtractingFirmware);
        extractFirmware();

    } else if(operationState() == InternalUpdateOperation::ExtractingFirmware) {
        setOperationState(InternalUpdateOperation::UploadingFimware);
        uploadFirmware();

    } else if(operationState() == InternalUpdateOperation::UploadingFimware) {
        setOperationState(InternalUpdateOperation::WaitingForUpdate);
        startUpdate();

    } else if(operationState() == InternalUpdateOperation::WaitingForUpdate) {
        finish();
    }
}

void InternalUpdateOperation::fetchFirmware()
{
    const auto target = deviceState()->deviceInfo().hardware.target;
    const auto fileInfo = m_versionInfo.fileInfo(QStringLiteral("update_tgz"), target);

    if(fileInfo.target() != target) {
        finishWithError(BackendError::DataError, QStringLiteral("Required file type or target not found"));
        return;
    }

    const auto fileName = QUrl(fileInfo.url()).fileName();
    m_firmwareFile = globalTempDirs->createFile(fileName, this);

    auto *fetcher = new RemoteFileFetcher(this);
    if(!fetcher->fetch(fileInfo, m_firmwareFile)) {
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

    deviceState()->setStatusString(QStringLiteral("Fetchig fimware update..."));
}

void InternalUpdateOperation::extractFirmware()
{
    auto *uncompressor = new TarZipUncompressor(m_firmwareFile, globalTempDirs->root(), this);

    connect(uncompressor, &TarZipUncompressor::finished, this, [=]() {
        uncompressor->deleteLater();
    });

    deviceState()->setStatusString(QStringLiteral("Extracting fimware update..."));
}

void InternalUpdateOperation::uploadFirmware()
{

}

void InternalUpdateOperation::startUpdate()
{

}
