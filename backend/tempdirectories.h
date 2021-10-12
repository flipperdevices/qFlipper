#pragma once

#include <QDir>

class TempDirectories
{
    TempDirectories();
    ~TempDirectories();

public:
    static TempDirectories *instance();

    QDir root() const;
    QDir subdir(const QString &subdirName) const;

private:
    QDir m_tempRoot;
};

#define tempDirs() (TempDirectories::instance())
