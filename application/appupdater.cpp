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
    m_state(State::Idle),
    m_progress(0)
{}

AppUpdater::State AppUpdater::state() const
{
    return m_state;
}

double AppUpdater::progress() const
{
    return m_progress;
}

void AppUpdater::installUpdate(const Flipper::Updates::VersionInfo &versionInfo)
{
#ifdef Q_OS_WINDOWS
    const auto fileInfo = versionInfo.fileInfo(QStringLiteral("installer"), QStringLiteral("windows/amd64"));
#elif defined(Q_OS_MAC)
    const auto fileInfo = versionInfo.fileInfo(QStringLiteral("dmg"), QStringLiteral("macos/amd64"));
#elif defined(Q_OS_LINUX)
    const auto fileInfo = versionInfo.fileInfo(QStringLiteral("AppImage"), QStringLiteral("linux/amd64"));
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
    auto *fetcher = new RemoteFileFetcher(this);

    const auto  cleanup = [=]() {
        file->deleteLater();
        fetcher->deleteLater();
    };

    connect(fetcher, &RemoteFileFetcher::finished, this, [=]() {
        info_msg("Application update download has finished.");
        setState(State::Updating);

        // IMPORTANT -- The file is closed automatically before renaming (https://doc.qt.io/qt-5/qfile.html#rename)
        if(!file->rename(filePath)) {
            error_msg(QStringLiteral("Failed to rename .part file: %1.").arg(file->fileName()));
            setState(State::ErrorOccured);

            file->remove();
            cleanup();
            return;
        }

    #if defined(Q_OS_LINUX)

        const auto executable = QFile::Permission::ExeUser  | QFile::Permission::ExeOwner |
                                QFile::Permission::ExeGroup | QFile::Permission::ExeOther;

        file->setPermissions(file->permissions() | executable);

    #endif

        cleanup();

        if(!performUpdate(filePath)) {
            error_msg("Failed to start application update.");
            setState(State::ErrorOccured);
        }
    });

    connect(fetcher, &RemoteFileFetcher::progressChanged, this, &AppUpdater::setProgress);

    if(!fetcher->fetch(fileInfo, file)) {
        error_msg("Failed to start the download.");
        setState(State::ErrorOccured);

        file->remove();
        cleanup();

    } else {
        info_msg("Downloading the application update...");
        setState(State::Downloading);
    }
}

void AppUpdater::setState(State state)
{
    if(m_state == state) {
        return;
    }

    m_state = state;
    emit stateChanged();
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
    const auto exitApplication = []() {
        info_msg("Update started, exiting the application...");
        QCoreApplication::exit(0);
    };

#if defined(Q_OS_WINDOWS)
    const auto success = QDesktopServices::openUrl(path);
    if(success) exitApplication();
    return success;

#elif defined(Q_OS_MAC)
    auto *mountDmg = new QProcess(this);
    mountDmg->setProgram(QStringLiteral("hdiutil"));
    mountDmg->setArguments({QStringLiteral("attach"), path});

    connect(mountDmg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus) {

        mountDmg->deleteLater();

        if(!exitCode && exitStatus == QProcess::NormalExit) {
            exitApplication();
        }
    });

    mountDmg->start();
    return mountDmg->error() == QProcess::UnknownError; //Really? no NoError code?

#elif defined(Q_OS_LINUX)
    const auto info = QFileInfo(path);
    const auto success = QProcess::startDetached(info.fileName(), {}, info.absoluteDir().absolutePath());

    if(success) exitApplication();
    return success;

#else
#error "Unsupported OS"
#endif
}
