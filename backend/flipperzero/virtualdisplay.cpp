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
    if(m_state != State::Stopped) {
        return;
    }

    setState(State::Starting);

    auto *operation = m_rpc->guiStartVirtualDisplay(firstFrame);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY) << "Failed to start virtual display:" << operation->errorString();
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
            qCDebug(LOG_VIRTDISPLAY) << "Failed to send screen frame:" << operation->errorString();
        }
    });
}

void VirtualDisplay::stop()
{
    if(m_state != State::Running) {
        return;
    }

    setState(State::Stopping);

    auto *operation = m_rpc->guiStopVirtualDisplay();
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY) << "Failed to stop virtual display:" << operation->errorString();
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
        emit started();
    } else if(m_state == State::Stopped) {
        emit stopped();
    }
}
