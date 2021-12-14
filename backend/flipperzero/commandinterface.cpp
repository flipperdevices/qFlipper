#include "commandinterface.h"

#include <QSerialPort>
#include <QLoggingCategory>

#include "flipperzero/devicestate.h"

#include "cli/systemfactoryresetoperation.h"
#include "cli/skipmotdoperation.h"
#include "cli/startrpcoperation.h"
#include "cli/stoprpcoperation.h"
#include "cli/systemrebootoperation.h"
#include "cli/storageremoveoperation.h"
#include "cli/storagemkdiroperation.h"
#include "cli/storagewriteoperation.h"
#include "cli/storagereadoperation.h"
#include "cli/storagelistoperation.h"
#include "cli/storagestatoperation.h"
#include "cli/storageinfooperation.h"

Q_LOGGING_CATEGORY(CATEGORY_CLI, "CLI");

using namespace Flipper;
using namespace Zero;

CommandInterface::CommandInterface(DeviceState *state, QObject *parent):
    AbstractOperationRunner(parent),
    m_serialPort(nullptr)
{
    // Automatically re-create serial port instance when a persistent device reconnects
    // TODO: move serial port instance to DeviceState
    const auto createSerialPort = [=]() {
        if(m_serialPort) {
            if(m_serialPort->isOpen()) {
                qCDebug(CATEGORY_CLI) << "Deleting a Serial Port instance that is still open.";
            }

            m_serialPort->deleteLater();
            m_serialPort = nullptr;
        }

        const auto &portInfo = state->deviceInfo().serialInfo;

        if(!portInfo.isNull()) {
            m_serialPort = new QSerialPort(portInfo, this);
        }
    };

    connect(state, &DeviceState::deviceInfoChanged, this, createSerialPort);

    createSerialPort();
}

StopRPCOperation *CommandInterface::stopRPCSession()
{
    return registerOperation(new StopRPCOperation(m_serialPort, this));
}

StartRPCOperation *CommandInterface::startRPCSession()
{
    return registerOperation(new StartRPCOperation(m_serialPort, this));
}

SystemRebootOperation *CommandInterface::rebootToOS()
{
    return registerOperation(new SystemRebootOperation(m_serialPort, SystemRebootOperation::RebootType::OS, this));
}

SystemRebootOperation *CommandInterface::rebootToRecovery()
{
    return registerOperation(new SystemRebootOperation(m_serialPort, SystemRebootOperation::RebootType::Recovery, this));
}

SystemFactoryResetOperation *CommandInterface::factoryReset()
{
    return registerOperation(new SystemFactoryResetOperation(m_serialPort, this));
}

StorageListOperation *CommandInterface::storageList(const QByteArray &path)
{
    return registerOperation(new StorageListOperation(m_serialPort, path, this));
}

StorageInfoOperation *CommandInterface::storageInfo(const QByteArray &path)
{
    return registerOperation(new StorageInfoOperation(m_serialPort, path, this));
}

StorageStatOperation *CommandInterface::storageStat(const QByteArray &path)
{
    return registerOperation(new StorageStatOperation(m_serialPort, path, this));
}

StorageReadOperation *CommandInterface::storageRead(const QByteArray &path, QIODevice *file)
{
    return registerOperation(new StorageReadOperation(m_serialPort, path, file, this));
}

StorageMkdirOperation *CommandInterface::storageMkdir(const QByteArray &path)
{
    return registerOperation(new StorageMkdirOperation(m_serialPort, path, this));
}

StorageWriteOperation *CommandInterface::storageWrite(const QByteArray &path, QIODevice *file)
{
    return registerOperation(new StorageWriteOperation(m_serialPort, path, file, this));
}

StorageRemoveOperation *CommandInterface::storageRemove(const QByteArray &path)
{
    return registerOperation(new StorageRemoveOperation(m_serialPort, path, this));
}

bool CommandInterface::onQueueStarted()
{
    const auto success = m_serialPort->open(QIODevice::ReadWrite);

    if(!success) {
        qCCritical(CATEGORY_CLI).noquote() <<  "Serial port error:" << m_serialPort->errorString();
    } else {
        enqueueOperation(new SkipMOTDOperation(m_serialPort, this));
        enqueueOperation(new StartRPCOperation(m_serialPort, this));
    }

    return success;
}

bool CommandInterface::onQueueFinished()
{
    connect(registerOperation(new StopRPCOperation(m_serialPort, this)), &AbstractOperation::finished,
            m_serialPort, &QSerialPort::close);

    return true;
}

const QLoggingCategory &CommandInterface::loggingCategory() const
{
    return CATEGORY_CLI();
}
