#include "appupdater.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QStandardPaths>

#include "macros.h"
#include "remotefilefetcher.h"

// TODO Refactor all this conditional compilation, it's ugly! (and inflexible).

AppUpdater::AppUpdater(QObject *parent):
    QObject(parent),
    m_progress(0)
{}

double AppUpdater::progress() const
{
    return m_progress;
}

void AppUpdater::installUpdate(const Flipper::Updates::VersionInfo &versionInfo)
{
#ifdef Q_OS_WINDOWS
    const auto fileInfo = versionInfo.fileInfo("installer", "windows/amd64");
#elif defined(Q_OS_MAC)
    const auto fileInfo = versionInfo.fileInfo("dmg", "macos/amd64");
#elif defined(Q_OS_LINUX)
    const auto fileInfo = versionInfo.fileInfo("AppImage", "linux/amd64");
#else
    #error "Unsupported OS"
#endif

    const auto fileName = QFileInfo(fileInfo.url()).fileName();

#if defined(Q_OS_WINDOWS) || defined(Q_OS_MAC)
    const auto filePath = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), fileName);
#elif defined(Q_OS_LINUX)
    const auto filePath = fileName;
#else
    #error "Unsupported OS"
#endif

    auto *file = new QFile(filePath + QStringLiteral(".part"));
    check_return_void(file->open(QIODevice::ReadWrite), "Failed to create file");

    auto *fetcher = new RemoteFileFetcher(this);

    const auto  cleanup = [=]() {
        file->deleteLater();
        fetcher->deleteLater();
    };

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        emit downloadFinished();

        // IMPORTANT -- The file is closed automatically before renaming (https://doc.qt.io/qt-5/qfile.html#rename)
        if(file->rename(fileName)) {
            error_msg("Failed to rename .part file to its proper name");
            emit errorOccured();
        }

        info_msg("Application update download has finished.");

    #if defined(Q_OS_LINUX)

        const auto executable = QFile::Permission::ExeUser  | QFile::Permission::ExeOwner |
                                QFile::Permission::ExeGroup | QFile::Permission::ExeOther;

        file->setPermissions(file->permissions() | executable);

    #endif

        if(performUpdate(filePath)) {
    // Seems too drastic for now, what if the user wants to keep previous versions?
    //#ifdef Q_OS_LINUX
    //        QFile::remove(QCoreApplication::applicationFilePath());
    //#endif
            QCoreApplication::exit(0);
        } else {
            error_msg("Failed to perform application update.");
            emit errorOccured();
        }

        cleanup();
    });

    connect(fetcher, &RemoteFileFetcher::progressChanged, this, &AppUpdater::setProgress);

    if(!fetcher->fetch(fileInfo, file)) {
        error_msg("Failed to fetch applicaton update file.");

        file->remove();
        cleanup();

        emit errorOccured();
    }
}

void AppUpdater::setProgress(double progress)
{
    if(qFuzzyCompare(m_progress, progress)) {
        return;
    }

    m_progress = progress;
    emit progressChanged();
}

bool AppUpdater::performUpdate(const QString &path)
{
#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
    const auto info = QFileInfo(path);
    return QProcess::startDetached(info.fileName(), {}, info.absoluteDir().absolutePath());
#elif defined(Q_OS_MAC)
    return QDesktopServices::openUrl(path);
#endif
}
