#include "radioupdateoperation.h"

#include <QFile>
#include <QTimer>
#include <QBuffer>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/radiomanifest.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/recovery.h"

#include "tarziparchive.h"
#include "tararchive.h"

using namespace Flipper;
using namespace Zero;

RadioUpdateOperation::RadioUpdateOperation(Recovery *recovery, QFile *file, QObject *parent):
    AbstractRecoveryOperation(recovery, parent),
    m_sourceFile(file),
    m_firmwareFile(new QBuffer(this)),
    m_loopTimer(new QTimer(this))
{
    connect(m_loopTimer, &QTimer::timeout, this, &RadioUpdateOperation::advanceOperationState);
}

const QString RadioUpdateOperation::description() const
{
    return QStringLiteral("Radio firmware update @%1").arg(deviceState()->name());
}

void RadioUpdateOperation::advanceOperationState()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(RadioUpdateOperation::UnpackingArchive);

    } else if(operationState() == RadioUpdateOperation::UnpackingArchive) {
        setOperationState(RadioUpdateOperation::StartingFUS);
        startFUS();

    } else if(operationState() == RadioUpdateOperation::StartingFUS) {
        setOperationState(RadioUpdateOperation::DeletingWirelessStack);
        deleteWirelessStack();

    } else if(operationState() == RadioUpdateOperation::DeletingWirelessStack) {
        if(isWirelessStackDeleted()) {
            setOperationState(RadioUpdateOperation::DownloadingWirelessStack);
            downloadWirelessStack();
        }

    } else if(operationState() == RadioUpdateOperation::DownloadingWirelessStack) {
        setOperationState(RadioUpdateOperation::UpgradingWirelessStack);
        upgradeWirelessStack();

    } else if(operationState() == RadioUpdateOperation::UpgradingWirelessStack) {
        if(isWirelessStackUpgraded()) {
            finish();
        }

    } else {
        finishWithError(QStringLiteral("Unexpected state"));
    }
}

void RadioUpdateOperation::unpackArchive()
{
//    auto *archive = new TarZipArchive(m_sourceFile, this);

//    if(archive->isError()) {
//        finishWithError(QStringLiteral("Failed to uncompress radio firmware file: %1").arg(archive->errorString()));
//        return;
//    }

//    connect(archive, &TarZipArchive::ready, this, [=]() {
    //    });
}

void RadioUpdateOperation::startFUS()
{
    if(!recovery()->startFUS()) {
        finishWithError(recovery()->errorString());
    }
}

void RadioUpdateOperation::deleteWirelessStack()
{
    if(!recovery()->deleteWirelessStack()) {
        finishWithError(recovery()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool RadioUpdateOperation::isWirelessStackDeleted()
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
        finishWithError(QStringLiteral("Failed to finish removal of the Wireless Stack."));
    }

    return !errorOccured;
}

void RadioUpdateOperation::downloadWirelessStack()
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        if(watcher->result()) {
            advanceOperationState();
        } else {
            finishWithError(QStringLiteral("Failed to download the Wireless Stack."));
        }

        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run(recovery(), &Recovery::downloadWirelessStack, m_firmwareFile, 0));
}

void RadioUpdateOperation::upgradeWirelessStack()
{
    if(!recovery()->upgradeWirelessStack()) {
        finishWithError(recovery()->errorString());
    } else {
        m_loopTimer->start(1000);
    }
}

bool RadioUpdateOperation::isWirelessStackUpgraded()
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
        finishWithError(QStringLiteral("Failed to finish installation of the Wireless Stack."));
    }

    return !errorOccured;
}
