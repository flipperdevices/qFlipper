#pragma once

#include <QDir>

class QFile;
class QObject;

class TempDirectories
{
    TempDirectories();
    ~TempDirectories();

public:
    static TempDirectories *instance();

    QDir root() const;
    QDir subdir(const QString &subdirName) const;

    QFile *createFile(const QString &fileName, QObject *parent = nullptr) const;

private:
    QDir m_tempRoot;
};

#define tempDirs() (TempDirectories::instance())
