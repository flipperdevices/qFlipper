#include "abstractoperation.h"

#include <QTimer>

#include "macros.h"

AbstractOperation::AbstractOperation(QObject *parent):
    QObject(parent),
    m_isError(false),
    m_errorString(QStringLiteral("No error")),
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

bool AbstractOperation::isError() const
{
    return m_isError;
}

const QString &AbstractOperation::errorString() const
{
    return m_errorString;
}

void AbstractOperation::onOperationTimeout()
{
    finishWithError(QStringLiteral("Operation timeout (generic)."));
}

void AbstractOperation::setState(int state)
{
    m_state = state;
}

void AbstractOperation::finishWithError(const QString &errorString)
{
    error_msg(errorString);

    m_isError = true;
    m_errorString = errorString;

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
