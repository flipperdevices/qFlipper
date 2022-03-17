#include "virtualdisplay.h"

#include <QDebug>
#include <QLoggingCategory>

#include "flipperzero.h"
#include "devicestate.h"
#include "protobufsession.h"

#include "rpc/guistartvirtualdisplayoperation.h"
#include "rpc/guistopvirtualdisplayoperation.h"
#include "rpc/guiscreenframeoperation.h"

Q_LOGGING_CATEGORY(LOG_VIRTDISPLAY, "DPY")

using namespace Flipper;
using namespace Zero;

VirtualDisplay::VirtualDisplay(QObject *parent):
    QObject(parent),
    m_displayState(DisplayState::Stopped),
    m_device(nullptr)
{}

void VirtualDisplay::setDevice(FlipperZero *device)
{
    if(!device) {
        return;
    }

    m_device = device;
}

VirtualDisplay::DisplayState VirtualDisplay::displayState() const
{
    return m_displayState;
}

void VirtualDisplay::start(const QByteArray &firstFrame)
{
    setDisplayState(DisplayState::Starting);

    auto *operation = m_device->rpc()->guiStartVirtualDisplay(firstFrame);
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
    auto *operation = m_device->rpc()->guiSendScreenFrame(screenFrame);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            qCDebug(LOG_VIRTDISPLAY).noquote() << "Failed to send screen frame:" << operation->errorString();
        }
    });
}

void VirtualDisplay::stop()
{
    setDisplayState(DisplayState::Stopping);

    auto *operation = m_device->rpc()->guiStopVirtualDisplay();

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
    emit displayStateChanged();
}
