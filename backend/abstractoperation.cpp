#include "abstractoperation.h"

#include <QTimer>

#include "macros.h"

AbstractOperation::AbstractOperation(QObject *parent):
    QObject(parent),
    m_timeout(new QTimer(this)),
    m_state(BasicState::Ready)
{
    connect(this, &AbstractOperation::finished, m_timeout, &QTimer::stop);
    connect(m_timeout, &QTimer::timeout, this, &AbstractOperation::onOperationTimeout);

    m_timeout->setSingleShot(true);
}

int AbstractOperation::state() const
{
    return m_state;
}

void AbstractOperation::onOperationTimeout()
{
    finishWithError(QStringLiteral("Operation timeout (generic)."));
}

void AbstractOperation::setState(int state)
{
    m_state = state;
}

void AbstractOperation::finishWithError(const QString &errorMsg)
{
    error_msg(errorMsg);

    setError(errorMsg);
    setState(BasicState::Finished);
    stopTimeout();

    finish();
}

void AbstractOperation::startTimeout(int msec)
{
    m_timeout->start(msec);
}

void AbstractOperation::stopTimeout()
{
    m_timeout->stop();
}
