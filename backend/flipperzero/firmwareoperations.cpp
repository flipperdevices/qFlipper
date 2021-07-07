#include "firmwareoperations.h"

#include <QTime>
#include <QThread>

#include "flipperzero/flipperzero.h"
#include "macros.h"

namespace Flipper {
namespace Zero {

FirmwareOperation::FirmwareOperation(FlipperZero *device):
    m_device(device)
{}

void FirmwareOperation::waitForReconnect(int timeoutMS)
{
    //TODO: Implement better syncronisation

    const auto now = QTime::currentTime();
    while(!m_device->isConnected() || (now.msecsTo(QTime::currentTime()) >= timeoutMS)) {
        QThread::msleep(100);
    }
}

FirmwareDownloadOperation::FirmwareDownloadOperation(FlipperZero *device, QIODevice *file):
    FirmwareOperation(device),
    m_file(file)
{
    m_device->setStatusMessage(QObject::tr("Pending"));
}

FirmwareDownloadOperation::~FirmwareDownloadOperation()
{
    m_file->deleteLater();
}

const QString FirmwareDownloadOperation::name() const
{
    return QString("Firmware Download to %1 %2").arg(m_device->model(), m_device->name());
}

bool FirmwareDownloadOperation::execute()
{
    m_device->setPersistent(true);

    if(!m_device->isDFU()) {
        check_return_bool(m_device->detach(), "Failed to detach device");
        waitForReconnect();
    }

    check_return_bool(m_device->downloadFirmware(m_file), "Failed to upload firmware");
    waitForReconnect();

    m_device->setPersistent(false);
    return true;
}

WirelessStackDownloadOperation::WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t addr):
    FirmwareOperation(device),
    m_file(file),
    m_targetAddress(addr)
{}

WirelessStackDownloadOperation::~WirelessStackDownloadOperation()
{
    m_file->deleteLater();
}

const QString WirelessStackDownloadOperation::name() const
{
    return QString("Radio Stack Download to %1 %2").arg(m_device->model(), m_device->name());
}

bool WirelessStackDownloadOperation::execute()
{
    m_device->setPersistent(true);

    if(!m_device->isDFU()) {
        check_return_bool(m_device->detach(), "Failed to detach device");
        waitForReconnect();
    }

    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::DFUOnly), "Failed to set device into DFU-only boot mode");
    waitForReconnect();

    check_return_bool(m_device->startFUS(), "Failed to start FUS");
    waitForReconnect();

    check_return_bool(m_device->downloadWirelessStack(m_file, m_targetAddress), "Failed to download wireless stack image");
    waitForReconnect();

    check_return_bool(m_device->startWirelessStack(), "Failed to start wireless stack");
    waitForReconnect();

    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::Normal), "Failed to set device into Normal boot mode");
    waitForReconnect();

    m_device->setPersistent(false);
    return true;
}

}
}
