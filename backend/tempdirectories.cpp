#include "tempdirectories.h"

#include <QFile>
#include <QTemporaryFile>
#include <QRandomGenerator>

#include "macros.h"

static QString randomString(int len)
{
   static const auto chars = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

   QString ret;

   for(int i = 0; i < len; ++i) {
       const auto index = QRandomGenerator::global()->bounded(chars.length());
       ret.append(chars.at(index));
   }

   return ret;
}

TempDirectories::TempDirectories():
    m_tempRoot(QDir::temp())
{
    const auto appDir = QStringLiteral("qFlipper-%1").arg(randomString(8));

    const auto success = m_tempRoot.mkdir(appDir) && m_tempRoot.cd(appDir);
    check_continue(success, "Failed to create temporary directory.");
}

TempDirectories::~TempDirectories()
{
    m_tempRoot.removeRecursively();
}

TempDirectories *TempDirectories::instance()
{
    static TempDirectories instance;
    return &instance;
}

QDir TempDirectories::root() const
{
    return m_tempRoot;
}

QDir TempDirectories::subdir(const QString &subdirName) const
{
    auto subdir = m_tempRoot;
    bool success;

    if(!m_tempRoot.exists(subdirName)) {
        success = subdir.mkdir(subdirName) && subdir.cd(subdirName);
    } else {
        success = subdir.cd(subdirName);
    }

    return success ? subdir : QDir();
}

QFile *TempDirectories::createFile(const QString &fileName, QObject *parent) const
{
    return new QFile(m_tempRoot.absoluteFilePath(fileName), parent);
}

QFile *TempDirectories::createTempFile() const
{
    return new QTemporaryFile(m_tempRoot.absoluteFilePath(QStringLiteral("temp.XXXXXXXX")));
}
