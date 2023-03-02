#include "fullrepairoperation.h"

#include <QFile>
#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/firmwaredownloadoperation.h"
#include "flipperzero/recovery/correctoptionbytesoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/assetsdownloadoperation.h"
#include "flipperzero/utility/regionprovisioningoperation.h"

#include "flipperzero/helper/firmwarehelper.h"

Q_DECLARE_LOGGING_CATEGORY(CATEGORY_DEFAULT)

using namespace Flipper;
using namespace Zero;

FullRepairOperation::FullRepairOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_versionInfo(versionInfo)
{}

const QString FullRepairOperation::description() const
{
    return QStringLiteral("Full Repair @%1").arg(deviceState()->name());
}

void FullRepairOperation::nextStateLogic()
{
    if(operationState() == FullRepairOperation::Ready) {
        setOperationState(FullRepairOperation::FetchingFirmware);
        fetchFirmware();

    } else if(operationState() == FullRepairOperation::FetchingFirmware) {
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
        setOperationState(FullRepairOperation::DownloadingAssets);
        downloadAssets();

    } else if(operationState() == FullRepairOperation::DownloadingAssets) {
        setOperationState(FullRepairOperation::ProvisioningRegion);
        provisionRegion();

    } else if(operationState() == FullRepairOperation::ProvisioningRegion) {
        setOperationState(FullRepairOperation::RestartingDevice);
        restartDevice();

    } else if(operationState() == FullRepairOperation::RestartingDevice) {
        finish();
    }
}

void FullRepairOperation::fetchFirmware()
{
    m_helper = new FirmwareHelper(deviceState(), m_versionInfo, this);

    connect(m_helper, &AbstractOperationHelper::finished, this, [=]() {
        if(m_helper->isError()) {
            finishWithError(m_helper->error(), QStringLiteral("Failed to fetch the files: %1").arg(m_helper->errorString()));
        } else {
            advanceOperationState();
        }
    });
}

void FullRepairOperation::setBootMode()
{
    registerSubOperation(m_recovery->setRecoveryBootMode());
}

void FullRepairOperation::downloadRadioFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::RadioFirmware);
    registerSubOperation(m_recovery->downloadWirelessStack(file));
}

void FullRepairOperation::downloadFirmware()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::Firmware);
    registerSubOperation(m_recovery->downloadFirmware(file));
}

void FullRepairOperation::correctOptionBytes()
{
    auto *file = m_helper->file(FirmwareHelper::FileIndex::OptionBytes);
    registerSubOperation(m_recovery->fixOptionBytes(file));
}

void FullRepairOperation::downloadAssets()
{
    if(deviceState()->isRecoveryMode()) {
        advanceOperationState();
        return;
    }

    auto *file = m_helper->file(FirmwareHelper::FileIndex::AssetsTgz);
    registerSubOperation(m_utility->downloadAssets(file));
}

void FullRepairOperation::provisionRegion()
{
    registerSubOperation(m_utility->provisionRegionData());
}

void FullRepairOperation::restartDevice()
{
    registerSubOperation(m_utility->restartDevice());
}

void FullRepairOperation::onSubOperationError(AbstractOperation *operation)
{
    if(operationState() == DownloadingRadioFirmware) {
        qCInfo(CATEGORY_DEFAULT) << operation->description() << "failed with reason:" << operation->errorString() << "Attempting to install the firmware anyway...";
        advanceOperationState();

    } else {
        AbstractTopLevelOperation::onSubOperationError(operation);
    }
}
