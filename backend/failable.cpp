#include "failable.h"

Failable::Failable()
{
    clearError();
}

bool Failable::isError() const
{
    return m_error != BackendError::NoError;
}

BackendError::ErrorType Failable::error() const
{
    return m_error;
}

void Failable::setError(BackendError::ErrorType error)
{
    m_error = error;
}

void Failable::setError(BackendError::ErrorType error, const QString &errorString)
{
    m_error = error;
    m_errorString = errorString;
}

const QString &Failable::errorString() const
{
    return m_errorString;
}

void Failable::setErrorString(const QString &errorMessage)
{
    m_error = BackendError::UnknownError;
    m_errorString = errorMessage;
}

void Failable::clearError()
{
    m_error = BackendError::NoError;
    m_errorString = QStringLiteral("No Error");
}
