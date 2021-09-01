#include "wirelessstackdownloadoperation.h"

#include <QTimer>
#include <QIODevice>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"

using namespace Flipper;
using namespace Zero;

WirelessStackDownloadOperation::WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file),
    m_loopTimer(new QTimer(this)),
    m_targetAddress(targetAddress)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Co-Processor firmware update pending..."));

    connect(m_loopTimer, &QTimer::timeout, this, &WirelessStackDownloadOperation::transitionToNextState);
}

WirelessStackDownloadOperation::~WirelessStackDownloadOperation()
{
    m_device->setPersistent(false);
    m_file->deleteLater();
}

const QString WirelessStackDownloadOperation::name() const
{
    return QString("Co-Processor Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
}

void WirelessStackDownloadOperation::start()
{
    if(state() != Ready) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    connect(m_device, &FlipperZero::isOnlineChanged, this, &WirelessStackDownloadOperation::transitionToNextState);
    transitionToNextState();
}

void WirelessStackDownloadOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        startTimeout();
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Ready) {
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
        setState(AbstractFirmwareOperation::Finished);
        finish();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void WirelessStackDownloadOperation::onOperationTimeout()
{
    switch(state()) {
    case WirelessStackDownloadOperation::BootingToDFU:
        setError(QStringLiteral("Failed to enter DFU mode: Operation timeout."));
        break;
    case WirelessStackDownloadOperation::SettingDFUBoot:
        setError(QStringLiteral("Failed to set DFU only boot mode: Operation timeout."));
        break;
    case WirelessStackDownloadOperation::StartingFUS:
        setError(QStringLiteral("Failed to start Firmware Upgrade Service: Operation timeout."));
        break;
    case WirelessStackDownloadOperation::DeletingWirelessStack:
        setError(QStringLiteral("Failed to delete existing Wireless Stack: Operation timeout."));
        break;
    case WirelessStackDownloadOperation::UpgradingWirelessStack:
        setError(QStringLiteral("Failed to upgrade Wireless Stack: Operation timeout."));
        break;
    case WirelessStackDownloadOperation::ResettingDFUBoot:
        setError(QStringLiteral("Failed to reset DFU only boot mode: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

void WirelessStackDownloadOperation::bootToDFU()
{
    if(m_device->isDFU()) {
        transitionToNextState();
    } else if(!m_device->bootToDFU()) {
        setError(QStringLiteral("Failed to enter DFU mode."));
    } else {}
}

void WirelessStackDownloadOperation::setDFUBoot(bool set)
{
    const auto bootMode = set ? FlipperZero::BootMode::DFUOnly : FlipperZero::BootMode::Normal;

    if(!m_device->setBootMode(bootMode)) {
        setError(QStringLiteral("Failed to set boot mode."));
    }
}

void WirelessStackDownloadOperation::startFUS()
{
    if(!m_device->startFUS()) {
        setError(QStringLiteral("Failed to start Firmware Upgrade Service."));
    }
}

void WirelessStackDownloadOperation::deleteWirelessStack()
{
    if(!m_device->deleteWirelessStack()) {
        setError(QStringLiteral("Failed to delete existing Wireless Stack."));
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackDeleted()
{
    const auto status = m_device->wirelessStatus();

    const auto waitNext = (status == FlipperZero::WirelessStatus::Invalid) ||
                          (status == FlipperZero::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == FlipperZero::WirelessStatus::WSRunning) ||
                              (status == FlipperZero::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        setError("Failed to finish removal of the Wireless Stack.");
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
            setError(QStringLiteral("Failed to download the Wireless Stack."));
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(m_device, &FlipperZero::downloadWirelessStack, m_file, m_targetAddress));
}

void WirelessStackDownloadOperation::upgradeWirelessStack()
{
    if(!m_device->upgradeWirelessStack()) {
        setError(QStringLiteral("Failed to start Wireless Stack upgrade."));
    } else {
        m_loopTimer->start(1000);
    }
}

bool WirelessStackDownloadOperation::isWirelessStackUpgraded()
{
    const auto status = m_device->wirelessStatus();

    const auto waitNext = (status == FlipperZero::WirelessStatus::Invalid) ||
                          (status == FlipperZero::WirelessStatus::UnhandledState);
    if(waitNext) {
        return false;
    }

    m_loopTimer->stop();

    const auto errorOccured = (status == FlipperZero::WirelessStatus::FUSRunning) ||
                              (status == FlipperZero::WirelessStatus::ErrorOccured);
    if(errorOccured) {
        setError("Failed to finish removal of the Wireless Stack.");
    }

    return !errorOccured;
}

void WirelessStackDownloadOperation::finish()
{
    disconnect(m_device, &FlipperZero::isOnlineChanged, this, &WirelessStackDownloadOperation::transitionToNextState);
    emit finished();
}
