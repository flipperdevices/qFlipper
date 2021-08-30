#include "abstractfirmwareoperation.h"

AbstractFirmwareOperation::AbstractFirmwareOperation(QObject *parent):
    QObject(parent),
    m_isError(false),
    m_errorString(QStringLiteral("No error")),
    m_state(BasicState::Idle)
{}

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
