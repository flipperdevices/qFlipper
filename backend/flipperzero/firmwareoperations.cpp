#include "firmwareoperations.h"

#include <QTime>
#include <QThread>

#include "flipperzero/flipperzero.h"
#include "macros.h"

namespace Flipper {
namespace Zero {

FirmwareDownloadOperation::FirmwareDownloadOperation(FlipperZero *device, QIODevice *file):
    m_device(device),
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
    }

    check_return_bool(m_device->downloadFirmware(m_file), "Failed to upload firmware");

    m_device->setPersistent(false);
    return true;
}

WirelessStackDownloadOperation::WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t addr):
    m_device(device),
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
    }

    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::DFUOnly), "Failed to set device into DFU-only boot mode");
    check_return_bool(m_device->startFUS(), "Failed to start FUS");
//    check_return_bool(m_device->isFUSRunning(), "FUS seemed to start, but isn't running anyway");

//    check_return_bool(m_device->eraseWirelessStack(), "Failed to begin erasing existing wireless stack");

//    while(!m_device->notFUSRunning()) {
//        info_msg("Waiting for wireless stack erase...");
//        QThread::msleep(1000);
//    }

//    check_return_bool(m_device->downloadWirelessStack(m_file, m_targetAddress), "Failed to download wireless stack image");
//    waitForReconnect();

//    check_return_bool(m_device->startWirelessStack(), "Failed to start wireless stack");
//    waitForReconnect();

    check_return_bool(m_device->setBootMode(FlipperZero::BootMode::Normal), "Failed to set device into Normal boot mode");

    m_device->setPersistent(false);
    return true;
}

}
}
