#pragma once

#include <QString>

class Failable
{
public:
    Failable();

    bool isError() const;
    const QString &errorString() const;
    void setErrorString(const QString &errorMessage);
    void resetErrorString();

private:
    bool m_isError;
    QString m_errorString;
};

