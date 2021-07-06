#include "firmwareoperations.h"
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
        waitForReconnect();
    }

    check_return_bool(m_device->downloadFirmware(m_file), "Failed to upload firmware");
    waitForReconnect();

    m_device->setPersistent(false);
    return true;
}

void FirmwareDownloadOperation::waitForReconnect()
{
    //TODO: Implement better syncronisation
    while(!m_device->isConnected()) {}
}

}
}
