#include "storageinforefreshoperation.h"

#include <cmath>

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"

#include "flipperzero/rpc/storageinfooperation.h"
#include "flipperzero/rpc/storagestatoperation.h"

using namespace Flipper;
using namespace Zero;

StorageInfoRefreshOperation::StorageInfoRefreshOperation(ProtobufSession *rpc, DeviceState *deviceState, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_storageInfo{}
{}

const QString StorageInfoRefreshOperation::description() const
{
    return QStringLiteral("Refresh Storage Info @%1").arg(deviceState()->name());
}

void StorageInfoRefreshOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(CheckingSDCard);
        checkSDCard();
    } else if(operationState() == CheckingSDCard) {
        setOperationState(CheckingManifest);
        checkManifest();
    } else if(operationState() == CheckingManifest) {
        updateStorageInfo();
        finish();
    }
}

void StorageInfoRefreshOperation::checkSDCard()
{
    auto *operation = rpc()->storageInfo(QByteArrayLiteral("/ext"));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check SD card: %1").arg(operation->errorString()));
            return;

        }

        m_storageInfo.isExternalPresent = operation->isPresent();

        if(m_storageInfo.isExternalPresent) {
            m_storageInfo.externalFree = floor((double)operation->sizeFree() * 100.0 /
                                               (double)operation->sizeTotal());
        } else {
            m_storageInfo.isAssetsInstalled = false;
            setOperationState(CheckingManifest);
        }

        advanceOperationState();
    });
}

void StorageInfoRefreshOperation::checkManifest()
{
    auto *operation = rpc()->storageStat(QByteArrayLiteral("/ext/Manifest"));

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(BackendError::InvalidDevice, QStringLiteral("Failed to check resource manifest: %1").arg(operation->errorString()));
            return;
        }

        m_storageInfo.isAssetsInstalled = operation->hasFile() && (operation->type() == StorageStatOperation::RegularFile);
        advanceOperationState();
    });
}

void StorageInfoRefreshOperation::updateStorageInfo()
{
    deviceState()->setStorageInfo(m_storageInfo);
}
