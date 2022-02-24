#include "virtualdisplay.h"

#include <QDebug>
#include <QLoggingCategory>

#include "devicestate.h"
#include "protobufsession.h"

#include "rpc/guistartvirtualdisplayoperation.h"
#include "rpc/guistopvirtualdisplayoperation.h"
#include "rpc/guiscreenframeoperation.h"

Q_LOGGING_CATEGORY(LOG_VIRTDISPLAY, "VIRTDISPLAY")

using namespace Flipper;
using namespace Zero;

VirtualDisplay::VirtualDisplay(DeviceState *deviceState, ProtobufSession *rpc, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_rpc(rpc),
    m_displayState(DisplayState::Stopped)
{}

void VirtualDisplay::start(const QByteArray &firstFrame)
{
    setDisplayState(DisplayState::Starting);

    auto *operation = m_rpc->guiStartVirtualDisplay(firstFrame);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to start virtual display:" << operation->errorString();
            setDisplayState(DisplayState::Stopped);
        } else {
            setDisplayState(DisplayState::Running);
        }
    });
}

void VirtualDisplay::sendFrame(const QByteArray &screenFrame)
{
    auto *operation = m_rpc->guiSendScreenFrame(screenFrame);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to send screen frame:" << operation->errorString();
        }
    });
}

void VirtualDisplay::stop()
{
    setDisplayState(DisplayState::Stopping);

    auto *operation = m_rpc->guiStopVirtualDisplay();

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to stop virtual display:" << operation->errorString();
        }

        setDisplayState(DisplayState::Stopped);
    });
}

void VirtualDisplay::setDisplayState(DisplayState newState)
{
    if(newState == m_displayState) {
        return;
    }

    m_displayState = newState;

    if(m_displayState == DisplayState::Running) {
        m_deviceState->setVirtualDisplayEnabled(true);
    } else if(m_displayState == DisplayState::Stopped) {
        m_deviceState->setVirtualDisplayEnabled(false);
    }
}
