#pragma once

#include <QUrl>
#include <QDir>
#include <QTemporaryDir>

class QFile;
class QObject;

class TempDirectories
{
    TempDirectories();

public:
    static TempDirectories *instance();

    QDir root() const;
    QDir subdir(const QString &subdirName) const;

    QUrl fileUrl(const QString &fileName) const;

    QFile *createFile(const QString &fileName, QObject *parent = nullptr) const;
    QFile *createTempFile(QObject *parent = nullptr) const;

private:
    QTemporaryDir m_root;
};

#define globalTempDirs (TempDirectories::instance())
