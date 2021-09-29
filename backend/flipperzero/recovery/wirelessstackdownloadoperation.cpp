#include "wirelessstackdownloadoperation.h"

#include <QTimer>
#include <QIODevice>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"
#include "flipperzero/recoveryinterface.h"

using namespace Flipper;
using namespace Zero;

WirelessStackDownloadOperation::WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress, QObject *parent):
    FlipperZeroOperation(device, parent),
    m_file(file),
    m_loopTimer(new QTimer(this)),
    m_targetAddress(targetAddress)
{
    device->setMessage(QStringLiteral("Co-Processor firmware update pending..."));

    connect(m_loopTimer, &QTimer::timeout, this, &WirelessStackDownloadOperation::transitionToNextState);
}

WirelessStackDownloadOperation::~WirelessStackDownloadOperation()
{
    m_file->deleteLater();
}

const QString WirelessStackDownloadOperation::description() const
{
    return QStringLiteral("Co-Processor Firmware Download @%1 %2").arg(device()->model(), device()->name());
}

void WirelessStackDownloadOperation::transitionToNextState()
{
    if(!device()->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractOperation::Ready) {
        setState(WirelessStackDownloadOperation::BootingToDFU);
        bootToDFU();

    } else if(state() == WirelessStackDownloadOperation::BootingToDFU) {
        setState(WirelessStackDownloadOperation::SettingDFUBoot);
        setDFUBoot(true);

    } else if(state() == WirelessStackDownloadOperation::SettingDFUBoot) {
        setState(WirelessStackDownloadOperation::StartingFUS);
        startFUS();

    } else if(state() == WirelessStackDownloadOperation::StartingFUS) {
        setState(WirelessStackDownloadOperation::DeletingWirelessStack);
        deleteWirelessStack();

    } else if(state() == WirelessStackDownloadOperation::DeletingWirelessStack) {
        if(isWirelessStackDeleted()) {
            setState(WirelessStackDownloadOperation::DownloadingWirelessStack);
            downloadWirelessStack();
        }

    } else if(state() == WirelessStackDownloadOperation::DownloadingWirelessStack) {
        setState(WirelessStackDownloadOperation::UpgradingWirelessStack);
        upgradeWirelessStack();

    } else if(state() == WirelessStackDownloadOperation::UpgradingWirelessStack) {
        if(isWirelessStackUpgraded()) {
            setState(WirelessStackDownloadOperation::ResettingDFUBoot);
            setDFUBoot(false);
        }

    } else if(state() == WirelessStackDownloadOperation::ResettingDFUBoot) {
        setState(AbstractOperation::Finished);
        finish();

    } else {
        finishWithError(QStringLiteral("Unexpected state."));
        device()->setError(errorString());
    }
}

void WirelessStackDownloadOperation::onOperationTimeout()
{
    QString msg;

    if(state() == WirelessStackDownloadOperation::BootingToDFU) {
        msg = QStringLiteral("Failed to enter DFU mode: Operation timeout.");
    } else if(state() == WirelessStackDownloadOperation::SettingDFUBoot) {
        msg = QStringLiteral("Failed to set DFU only boot mode: Operation timeout.");
    } else if(state() == WirelessStackDownloadOperation::StartingFUS) {
        msg = QStringLiteral("Failed to start Firmware Upgrade Service: Operation timeout.");
    } else if(state() == WirelessStackDownloadOperation::DeletingWirelessStack) {
        msg = QStringLiteral("Failed to delete existing Wireless Stack: Operation timeout.");
    } else if(state() == WirelessStackDownloadOperation::UpgradingWirelessStack) {
        msg = QStringLiteral("Failed to upgrade Wireless Stack: Operation timeout.");
    } else if(state() == WirelessStackDownloadOperation::ResettingDFUBoot) {
        msg = QStringLiteral("Failed to reset DFU only boot mode: Operation timeout.");
    } else {
        msg = QStringLiteral("Should not have timed out here, probably a bug.");
    }

    finishWithError(msg);
    device()->setError(errorString());
}

void WirelessStackDownloadOperation::bootToDFU()
{
    if(device()->isDFU()) {
        transitionToNextState();
    } else if(!device()->bootToDFU()) {
        finishWithError(device()->errorString());
    } else {}
}

void WirelessStackDownloadOperation::setDFUBoot(bool set)
{
    const auto bootMode = set ? RecoveryInterface::BootMode::DFUOnly : RecoveryInterface::BootMode::Normal;

    if(!device()->recovery()->setBootMode(bootMode)) {
        finishWithError(device()->errorString());
    }
}

void WirelessStackDownloadOperation::startFUS()
{
    if(!device()->recovery()->startFUS()) {
        finishWithError(device()->errorString());
    }
}

void WirelessStackDownloadOperation::deleteWirelessStack()
{
    if(!device()->recovery()->deleteWirelessStack()) {
        finishWithError(device()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackDeleted()
{
    const auto status = device()->recovery()->wirelessStatus();

    const auto waitNext = (status == RecoveryInterface::WirelessStatus::Invalid) ||
                          (status == RecoveryInterface::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == RecoveryInterface::WirelessStatus::WSRunning) ||
                              (status == RecoveryInterface::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        finishWithError(QStringLiteral("Failed to finish removal of the Wireless Stack."));
        device()->setError(errorString());
    }

    return !errorOccured;
}

void WirelessStackDownloadOperation::downloadWirelessStack()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            transitionToNextState();
        } else {
            finishWithError(QStringLiteral("Failed to download the Wireless Stack."));
            device()->setError(errorString());
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(device()->recovery(), &RecoveryInterface::downloadWirelessStack, m_file, m_targetAddress));
}

void WirelessStackDownloadOperation::upgradeWirelessStack()
{
    if(!device()->recovery()->upgradeWirelessStack()) {
        finishWithError(device()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackUpgraded()
{
    const auto status = device()->recovery()->wirelessStatus();

    const auto waitNext = (status == RecoveryInterface::WirelessStatus::Invalid) ||
                          (status == RecoveryInterface::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == RecoveryInterface::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        finishWithError(QStringLiteral("Failed to finish installation of the Wireless Stack."));
        device()->setError(errorString());
    }

    return !errorOccured;
}
