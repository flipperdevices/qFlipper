#include "internalupdateoperation.h"

//#include <QFile>
//#include <QTemporaryFile>

#include "flipperzero/devicestate.h"

#include "flipperzero/utilityinterface.h"
//#include "flipperzero/utility/restartoperation.h"
//#include "flipperzero/helper/firmwarehelper.h"

//#include "tempdirectories.h"

using namespace Flipper;
using namespace Zero;

InternalUpdateOperation::InternalUpdateOperation(UtilityInterface *utility, DeviceState *state, const Updates::VersionInfo &versionInfo, QObject *parent):
    Flipper::Zero::AbstractTopLevelOperation(state, parent),
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

}

void InternalUpdateOperation::uploadFirmware()
{

}

void InternalUpdateOperation::startUpdate()
{

}

void InternalUpdateOperation::onSubOperationError(AbstractOperation *operation)
{
//    const auto keepError = operationState() == FullUpdateOperation::SavingBackup ||
//                           operationState() == FullUpdateOperation::StartingRecovery;

//    finishWithError(keepError ? operation->error() : BackendError::OperationError, operation->errorString());
    finishWithError(operation->error(), operation->errorString());
}
