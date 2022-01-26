#include "failable.h"

Failable::Failable()
{
    resetErrorString();
}

bool Failable::isError() const
{
    return m_isError;
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

void Failable::resetErrorString()
{
    m_isError = false;
    m_errorString = QStringLiteral("No Error");
}
