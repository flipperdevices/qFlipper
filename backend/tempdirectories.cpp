#include "tempdirectories.h"

#include <QFile>
#include <QDateTime>
#include <QTemporaryFile>

TempDirectories::TempDirectories():
    m_root(QDir::temp().absoluteFilePath(QStringLiteral("%1-XXXXXXXX").arg(APP_NAME)))
{}

TempDirectories *TempDirectories::instance()
{
    static TempDirectories instance;
    return &instance;
}

QDir TempDirectories::root() const
{
    return QDir(m_root.path());
}

QDir TempDirectories::subdir(const QString &subdirName) const
{
    auto subdir = root();
    bool success;

    if(!root().exists(subdirName)) {
        success = subdir.mkdir(subdirName) && subdir.cd(subdirName);
    } else {
        success = subdir.cd(subdirName);
    }

    return success ? subdir : QDir();
}

QUrl TempDirectories::fileUrl(const QString &fileName) const
{
    return QUrl::fromLocalFile(root().absoluteFilePath(fileName));
}

QFile *TempDirectories::createFile(const QString &fileName, QObject *parent) const
{
    return new QFile(root().absoluteFilePath(fileName), parent);
}

QFile *TempDirectories::createTempFile(QObject *parent) const
{
    return new QTemporaryFile(root().absoluteFilePath(QStringLiteral("temp-XXXXXXXX")), parent);
}
