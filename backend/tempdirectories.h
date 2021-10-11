#pragma once

#include <QDir>

class TempDirectories
{
    TempDirectories();
    ~TempDirectories();

public:
    static TempDirectories *instance();

    QDir tempRoot() const;
    QDir tempSubdir(const QString &subdirName) const;

private:
    QDir m_tempRoot;
};

