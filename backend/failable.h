#pragma once

#include <QString>

class Failable
{
    bool m_isError;
    QString m_errorString;

public:
    Failable():
        m_isError(false),
        m_errorString(QStringLiteral("No Error"))
    {}

    bool isError() const { return m_isError; }
    const QString &errorString() const { return m_errorString; }

    void setError(const QString &errorMessage) {
        m_isError = true;
        m_errorString = errorMessage;
    }
};

