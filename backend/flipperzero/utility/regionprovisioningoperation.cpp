#include "regionprovisioningoperation.h"

#include <QDebug>
#include <QTimer>
#include <QLoggingCategory>

#include "regioninfo.h"
#include "tempdirectories.h"
#include "remotefilefetcher.h"

#include "flipperzero/devicestate.h"

#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagewriteoperation.h"


Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEBUG)

using namespace Flipper;
using namespace Zero;

RegionProvisioningOperation::RegionProvisioningOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_regionInfoFile(globalTempDirs->createTempFile(this)),
    m_regionDataFile(globalTempDirs->createTempFile(this))
{}

const QString RegionProvisioningOperation::description() const
{
    return QStringLiteral("Region Provisioning @%1").arg(deviceState()->deviceInfo().name);
}

void RegionProvisioningOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(FetchingRegionInfo);
        fetchRegionInfo();

    } else if(operationState() == FetchingRegionInfo) {
        setOperationState(GeneratingRegionData);
        generateRegionData();

    } else if(operationState() == GeneratingRegionData) {
        setOperationState(UploadingRegionData);
        uploadRegionData();

    } else if(operationState() == UploadingRegionData) {
        finish();
    }
}

void RegionProvisioningOperation::fetchRegionInfo()
{
    static const auto apiUrl = QStringLiteral("https://update.flipperzero.one/regions/api/v0/bundle");
    auto *fetcher = new RemoteFileFetcher(apiUrl, m_regionInfoFile, this);

    if(fetcher->isError()) {
        finishWithError(fetcher->error(), QStringLiteral("Failed to fetch region info file: %1").arg(fetcher->errorString()));
        return;
    }

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        if(fetcher->isError()) {
            finishWithError(fetcher->error(), QStringLiteral("Failed to fetch region info file: %1").arg(fetcher->errorString()));
        } else {
            advanceOperationState();
        }
    });
}

void RegionProvisioningOperation::generateRegionData()
{
    if(!m_regionInfoFile->open(QIODevice::ReadOnly)) {
        finishWithError(BackendError::DiskError, m_regionInfoFile->errorString());
        return;
    }

    const RegionInfo regionInfo(m_regionInfoFile->readAll());

    if(!regionInfo.isValid()) {
        finishWithError(BackendError::DataError, QStringLiteral("Server returned invalid data"));
        return;

    } else if(regionInfo.isError()) {
        // TODO: decide on proper behaviour in this case
        finishWithError(BackendError::DataError, regionInfo.errorString());
        return;
    }

    const auto countryCode = regionInfo.detectedCountry();
    const auto bandKeys = countryCode.isEmpty() ? regionInfo.defaultBandKeys() : regionInfo.countryBandKeys(countryCode);

    qCDebug(CATEGORY_DEBUG) << "Detected region:" << countryCode;
    qCDebug(CATEGORY_DEBUG) << "Allowed bands:" << bandKeys;

    if(!m_regionDataFile->open(QIODevice::WriteOnly)) {
        finishWithError(BackendError::DiskError, m_regionInfoFile->errorString());
        return;
    }

    m_regionDataFile->write("Hello there!");
    m_regionDataFile->close();

    advanceOperationState();
}

void RegionProvisioningOperation::uploadRegionData()
{
    auto *operation = rpc()->storageWrite("/int/.region_data", m_regionDataFile);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            advanceOperationState();
        }
    });
}
