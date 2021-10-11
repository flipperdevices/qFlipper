#include "tempdirectories.h"

#include "macros.h"

TempDirectories::TempDirectories():
    m_tempRoot(QDir::temp())
{
    const auto appDir = QStringLiteral("qFlipper");
    const auto success = m_tempRoot.mkdir(appDir) && m_tempRoot.cd(appDir);
    check_continue(success, "Failed to create a temporary directory");
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

QDir TempDirectories::tempRoot() const
{
    return m_tempRoot;
}

QDir TempDirectories::tempSubdir(const QString &subdirName) const
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
