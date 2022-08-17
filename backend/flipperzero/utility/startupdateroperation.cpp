#include "startupdateroperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/systemrebootoperation.h"
#include "flipperzero/rpc/systemupdateoperation.h"

using namespace Flipper;
using namespace Zero;

StartUpdaterOperation::StartUpdaterOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &manifestPath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_manifestPath(manifestPath)
{
    // Waiting for the whole update process with no feedback
    setTimeout(1000*60*10);
}

const QString StartUpdaterOperation::description() const
{
    return QStringLiteral("Start Update @%1").arg(deviceState()->deviceInfo().name);
}

void StartUpdaterOperation::onOperationTimeout()
{
    finishWithError(BackendError::UnknownError, QStringLiteral("Failed to update: timeout exceeded"));
}

void StartUpdaterOperation::onDeviceOnlineChanged()
{
    if(deviceState()->isOnline()) {
        advanceOperationState();
    } else {
        startTimeout();
    }
}

void StartUpdaterOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(StartUpdaterOperation::StartingUpdate);
        startUpdate();

    } else if(operationState() == StartUpdaterOperation::StartingUpdate) {
        setOperationState(StartUpdaterOperation::RebootingToUpdater);
        rebootToUpdater();

    } else if(operationState() == StartUpdaterOperation::RebootingToUpdater) {
        setOperationState(StartUpdaterOperation::WaitingForUpdate);
        waitForUpdate();

    } else if(operationState() == StartUpdaterOperation::WaitingForUpdate) {
        finish();
    }
}

void StartUpdaterOperation::startUpdate()
{
    deviceState()->setProgress(-1.0);
    deviceState()->setStatusString(QStringLiteral("Initiating update procedure ..."));

    auto *operation = rpc()->systemUpdate(m_manifestPath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else if(!operation->isResultOk()) {
            finishWithError(BackendError::UpdaterError, operation->resultString());
        } else {
            advanceOperationState();
        }
    });
}

void StartUpdaterOperation::rebootToUpdater()
{
    deviceState()->setStatusString(QStringLiteral("Rebooting to updater ..."));

    auto *operation = rpc()->rebootToUpdater();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            advanceOperationState();
        }
    });
}

void StartUpdaterOperation::waitForUpdate()
{
    deviceState()->setStatusString(QStringLiteral("Updating, follow the progress on your Flipper Zero"));

    connect(deviceState(), &DeviceState::isOnlineChanged, this, &StartUpdaterOperation::onDeviceOnlineChanged);
    startTimeout();
}
