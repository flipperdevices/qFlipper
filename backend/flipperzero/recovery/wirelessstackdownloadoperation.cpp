#include "wirelessstackdownloadoperation.h"

#include <QDebug>
#include <QTimer>
#include <QIODevice>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

Q_DECLARE_LOGGING_CATEGORY(LOG_RECOVERY)

using namespace Flipper;
using namespace Zero;

WirelessStackDownloadOperation::WirelessStackDownloadOperation(Recovery *recovery, QIODevice *file, uint32_t targetAddress, QObject *parent):
    AbstractRecoveryOperation(recovery, parent),
    m_file(file),
    m_loopTimer(new QTimer(this)),
    m_targetAddress(targetAddress),
    m_retryCount(3)
{
    connect(m_loopTimer, &QTimer::timeout, this, &WirelessStackDownloadOperation::nextStateLogic);
}

const QString WirelessStackDownloadOperation::description() const
{
    return QStringLiteral("Co-Processor Firmware Download @%1").arg(deviceState()->name());
}

void WirelessStackDownloadOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(WirelessStackDownloadOperation::StartingFUS);
        startFUS();

    } else if(operationState() == WirelessStackDownloadOperation::StartingFUS) {
        setOperationState(WirelessStackDownloadOperation::DeletingWirelessStack);
        deleteWirelessStack();

    } else if(operationState() == WirelessStackDownloadOperation::DeletingWirelessStack) {
        if(isWirelessStackDeleted()) {
            setOperationState(WirelessStackDownloadOperation::DownloadingWirelessStack);
            downloadWirelessStack();
        }

    } else if(operationState() == WirelessStackDownloadOperation::DownloadingWirelessStack) {
        setOperationState(WirelessStackDownloadOperation::UpgradingWirelessStack);
        upgradeWirelessStack();

    } else if(operationState() == WirelessStackDownloadOperation::UpgradingWirelessStack) {
        if(!isWirelessStackUpgraded()) {
            return;

        } else if(!isWirelessStackOK()) {
            setOperationState(AbstractOperation::Ready);
            tryAgain();

        } else {
            finish();
        }
    }
}

void WirelessStackDownloadOperation::onOperationTimeout()
{
    QString msg;

    if(operationState() == WirelessStackDownloadOperation::StartingFUS) {
        msg = QStringLiteral("Failed to start Firmware Upgrade Service: Operation timeout.");
    } else if(operationState() == WirelessStackDownloadOperation::DeletingWirelessStack) {
        msg = QStringLiteral("Failed to delete existing Wireless Stack: Operation timeout.");
    } else if(operationState() == WirelessStackDownloadOperation::UpgradingWirelessStack) {
        msg = QStringLiteral("Failed to upgrade Wireless Stack: Operation timeout.");
    } else {
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    if(!deviceState()->isOnline()) {
        finishWithError(BackendError::RecoveryError, msg);
    } else {
        qCDebug(LOG_RECOVERY) << "Timeout with an online device, assuming it is still functional";
        advanceOperationState();
    }
}

void WirelessStackDownloadOperation::startFUS()
{
    if(!recovery()->startFUS()) {
        finishWithError(BackendError::RecoveryError, recovery()->errorString());
    } else {
        startTimeout();
    }
}

void WirelessStackDownloadOperation::deleteWirelessStack()
{
    if(!recovery()->deleteWirelessStack()) {
        finishWithError(BackendError::RecoveryError, recovery()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackDeleted()
{
    const auto status = recovery()->wirelessStatus();

    const auto waitNext = (status == Recovery::WirelessStatus::Invalid) ||
                          (status == Recovery::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == Recovery::WirelessStatus::WSRunning) ||
                              (status == Recovery::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        finishWithError(BackendError::RecoveryError, QStringLiteral("Failed to finish removal of the Wireless Stack."));
    }

    return !errorOccured;
}

void WirelessStackDownloadOperation::downloadWirelessStack()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            advanceOperationState();
        } else {
            finishWithError(BackendError::RecoveryError, QStringLiteral("Failed to download the Wireless Stack."));
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(recovery(), &Recovery::downloadWirelessStack, m_file, m_targetAddress));
}

void WirelessStackDownloadOperation::upgradeWirelessStack()
{
    if(!recovery()->upgradeWirelessStack()) {
        finishWithError(BackendError::RecoveryError, recovery()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackUpgraded()
{
    const auto status = recovery()->wirelessStatus();

    const auto waitNext = (status == Recovery::WirelessStatus::Invalid) ||
                          (status == Recovery::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == Recovery::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        finishWithError(BackendError::RecoveryError, QStringLiteral("Failed to finish installation of the Wireless Stack."));
    }

    return !errorOccured;
}

bool WirelessStackDownloadOperation::isWirelessStackOK()
{
    return recovery()->checkWirelessStack();
}

void WirelessStackDownloadOperation::tryAgain()
{
    if(--m_retryCount == 0) {
        finishWithError(BackendError::RecoveryError, QStringLiteral("Could not install wireless stack after several tries, giving up"));

    } else {
        qCDebug(LOG_RECOVERY) << "Wireless stack installation seems to have failed, retrying...";
        advanceOperationState();
    }
}
