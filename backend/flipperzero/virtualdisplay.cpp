#include "virtualdisplay.h"

#include <QDebug>
#include <QLoggingCategory>

#include "devicestate.h"
#include "commandinterface.h"

#include "rpc/guistartvirtualdisplayoperation.h"
#include "rpc/guistopvirtualdisplayoperation.h"
#include "rpc/guiscreenframeoperation.h"

Q_LOGGING_CATEGORY(LOG_VIRTDISPLAY, "VIRTDISPLAY")

using namespace Flipper;
using namespace Zero;

VirtualDisplay::VirtualDisplay(DeviceState *deviceState, CommandInterface *rpc, QObject *parent):
    QObject(parent),
    m_deviceState(deviceState),
    m_rpc(rpc),
    m_state(State::Stopped)
{}

void VirtualDisplay::start(const QByteArray &firstFrame)
{
    setState(State::Starting);

    auto *operation = m_rpc->guiStartVirtualDisplay(firstFrame);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to start virtual display:" << operation->errorString();
            setState(State::Stopped);
        } else {
            setState(State::Running);
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
    setState(State::Stopping);

    auto *operation = m_rpc->guiStopVirtualDisplay();
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to stop virtual display:" << operation->errorString();
        }

        setState(State::Stopped);
    });
}

void VirtualDisplay::setState(State newState)
{
    if(newState == m_state) {
        return;
    }

    m_state = newState;

    if(m_state == State::Running) {
        m_deviceState->setVirtualDisplayEnabled(true);
    } else if(m_state == State::Stopped) {
        m_deviceState->setVirtualDisplayEnabled(false);
    }
}
