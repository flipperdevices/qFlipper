#include "firmwareoperations.h"

#include <QTime>
#include <QThread>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"
#include "macros.h"

namespace Flipper {
namespace Zero {

FirmwareDownloadOperation::FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent):
    AbstractFirmwareOperation(parent),
    m_device(device),
    m_file(file)
{
    m_device->setPersistent(true);
    m_device->setStatusMessage(QObject::tr("Firmware download pending..."));
}

FirmwareDownloadOperation::~FirmwareDownloadOperation()
{
    m_device->setPersistent(false);
    m_file->deleteLater();
}

const QString FirmwareDownloadOperation::name() const
{
    return QStringLiteral("Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
}

void FirmwareDownloadOperation::start()
{
    if(state() != Idle) {
        setError(QStringLiteral("Trying to start an operation that is either already running or has finished."));
        return;
    }

    transitionToNextState();
}

void FirmwareDownloadOperation::transitionToNextState()
{
    if(!m_device->isOnline()) {
        return;
    }

    stopTimeout();

    if(state() == AbstractFirmwareOperation::Idle) {
        setState(State::WaitForDFU);

        connect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);

        if(m_device->isDFU()) {
            transitionToNextState();
        } else if(!m_device->enterDFU()) {
            setError(QStringLiteral("Failed to detach the device."));
            return;
        } else {
            startTimeout();
        }

    } else if(state() == State::WaitForDFU) {
        setState(State::Downloading);

        auto *watcher = new QFutureWatcher<bool>(this);

        connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
            if(watcher->result()) {
                transitionToNextState();
            } else {
                setError(QStringLiteral("Failed to download the firmware."));
            }

            watcher->deleteLater();
        });

        watcher->setFuture(QtConcurrent::run(m_device, &FlipperZero::downloadFirmware, m_file));

    } else if(state() == State::Downloading) {
        setState(State::WaitForVCP);

        if(!m_device->leaveDFU()) {
            setError(QStringLiteral("Failed to leave DFU mode."));
        } else {
            startTimeout();
        }

    } else if(state() == State::WaitForVCP) {
        setState(AbstractFirmwareOperation::Finished);

        disconnect(m_device, &FlipperZero::isOnlineChanged, this, &FirmwareDownloadOperation::transitionToNextState);
        emit finished();

    } else {
        setError(QStringLiteral("Unexpected state."));
    }
}

void FirmwareDownloadOperation::onOperationTimeout()
{
    switch(state()) {
    case FirmwareDownloadOperation::WaitForDFU:
        setError(QStringLiteral("Failed to reach DFU mode: Operation timeout."));
        break;
    case FirmwareDownloadOperation::WaitForVCP:
        setError(QStringLiteral("Failed to reboot the device: Operation timeout."));
        break;
    default:
        setError(QStringLiteral("Should not have timed out here, probably a bug."));
    }
}

//WirelessStackDownloadOperation::WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress):
//    m_device(device),
//    m_file(file),
//    m_targetAddress(targetAddress)
//{
//    m_device->setPersistent(true);
//    m_device->setStatusMessage(QObject::tr("Co-processor firmware update pending..."));
//}

//WirelessStackDownloadOperation::~WirelessStackDownloadOperation()
//{
//    m_device->setPersistent(false);
//    m_file->deleteLater();
//}

//const QString WirelessStackDownloadOperation::name() const
//{
//    return QString("Coprocessor Firmware Download @%1 %2").arg(m_device->model(), m_device->name());
//}

//bool WirelessStackDownloadOperation::execute()
//{
//    if(!m_device->isDFU()) {
//        check_return_bool(m_device->detach(), "Failed to detach device");
//    }

//    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::DFUOnly), "Failed to set device into DFU-only boot mode");
//    check_return_bool(m_device->startFUS(), "Failed to start FUS");
//    check_return_bool(m_device->isFUSRunning(), "FUS seemed to start, but isn't running anyway");
//    check_return_bool(m_device->deleteWirelessStack(), "Failed to erase existing wireless stack");
//    check_return_bool(m_device->downloadWirelessStack(m_file, m_targetAddress), "Failed to download wireless stack image");
//    check_return_bool(m_device->upgradeWirelessStack(), "Failed to upgrade wireless stack");
//    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::Normal), "Failed to set device into Normal boot mode");

//    return true;
//}

//FixBootIssuesOperation::FixBootIssuesOperation(FlipperZero *device):
//    m_device(device)
//{
//    m_device->setPersistent(true);
//    m_device->setStatusMessage(QObject::tr("Fix boot issues operation pending..."));
//}

//FixBootIssuesOperation::~FixBootIssuesOperation()
//{
//    m_device->setPersistent(false);
//}

//const QString FixBootIssuesOperation::name() const
//{
//    return QString("Fix boot issues @%1 %2").arg(m_device->model(), m_device->name());
//}

//bool FixBootIssuesOperation::execute()
//{
//    check_return_bool(m_device->startWirelessStack(), "Failed to start wireless stack");
//    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::Normal), "Failed to set device into Normal boot mode");
//    return true;
//}

//FixOptionBytesOperation::FixOptionBytesOperation(FlipperZero *device, QIODevice *file):
//    m_device(device),
//    m_file(file)
//{
//    m_device->setPersistent(true);
//    m_device->setStatusMessage(QObject::tr("Check Option Bytes operation pending..."));
//}

//FixOptionBytesOperation::~FixOptionBytesOperation()
//{
//    m_device->setPersistent(false);
//    m_file->deleteLater();
//}

//const QString FixOptionBytesOperation::name() const
//{
//    return QString("Fix Option Bytes @%1 %2").arg(m_device->model(), m_device->name());
//}

//bool FixOptionBytesOperation::execute()
//{
//    if(!m_device->isDFU()) {
//        check_return_bool(m_device->detach(), "Failed to detach device");
//    }

//    check_return_bool(m_device->fixOptionBytes(m_file), "Failed to check option bytes");
//    return true;
//}

}}
