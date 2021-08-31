#include "firmwareoperations.h"

#include <QTime>
#include <QThread>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "flipperzero/flipperzero.h"
#include "macros.h"

namespace Flipper {
namespace Zero {

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

}}
