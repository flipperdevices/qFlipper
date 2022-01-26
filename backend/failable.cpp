#include "failable.h"

Failable::Failable()
{
    resetError();
}

bool Failable::isError() const
{
    return m_isError;
}

BackendError::ErrorType Failable::error() const
{
    return m_error;
}

void Failable::setError(BackendError::ErrorType error)
{
    m_isError = true;
    m_error = error;
}

void Failable::setError(BackendError::ErrorType error, const QString &errorString)
{
    m_isError = true;
    m_error = error;
    m_errorString = errorString;
}

const QString &Failable::errorString() const
{
    return m_errorString;
}

void Failable::setErrorString(const QString &errorMessage)
{
    m_isError = true;
    m_errorString = errorMessage;
}

void Failable::resetError()
{
    m_isError = false;
    m_error = BackendError::UnknownError;
    m_errorString = QStringLiteral("No Error");
}
