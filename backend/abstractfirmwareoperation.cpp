#include "abstractfirmwareoperation.h"

#include <QTimer>

AbstractFirmwareOperation::AbstractFirmwareOperation(QObject *parent):
    QObject(parent),
    m_isError(false),
    m_errorString(QStringLiteral("No error")),
    m_timeout(new QTimer(this)),
    m_state(BasicState::Idle)
{
    connect(this, &AbstractFirmwareOperation::finished, m_timeout, &QTimer::stop);
    connect(m_timeout, &QTimer::timeout, this, &AbstractFirmwareOperation::onOperationTimeout);

    m_timeout->setSingleShot(true);
}

int AbstractFirmwareOperation::state() const
{
    return m_state;
}

bool AbstractFirmwareOperation::isError() const
{
    return m_isError;
}

const QString &AbstractFirmwareOperation::errorString() const
{
    return m_errorString;
}

void AbstractFirmwareOperation::onOperationTimeout()
{
    setError(QStringLiteral("Operation timeout (generic)."));
}

void AbstractFirmwareOperation::setState(int state)
{
    m_state = state;
}

void AbstractFirmwareOperation::setError(const QString &errorString)
{
    m_isError = true;
    m_errorString = errorString;

    setState(BasicState::Finished);
    emit finished();
}

void AbstractFirmwareOperation::startTimeout(int msec)
{
    m_timeout->start(msec);
}

void AbstractFirmwareOperation::stopTimeout()
{
    m_timeout->stop();
}
