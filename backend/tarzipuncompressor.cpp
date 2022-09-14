#include "tarzipuncompressor.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "tararchive.h"
#include "tarziparchive.h"
#include "tempdirectories.h"

TarZipUncompressor::TarZipUncompressor(QFile *tarZipFile, const QDir &targetDir, QObject *parent):
    QObject(parent),
    m_tarZipArchive(new TarZipArchive(tarZipFile, this)),
    m_targetDir(targetDir)
{
    connect(m_tarZipArchive, &TarZipArchive::ready, this, &TarZipUncompressor::onArchiveReady);
}

void TarZipUncompressor::onArchiveReady()
{
    auto *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, &TarZipUncompressor::finished);
#if QT_VERSION < 0x060000
    watcher->setFuture(QtConcurrent::run(this, &TarZipUncompressor::extractFiles));
#else
    watcher->setFuture(QtConcurrent::run(&TarZipUncompressor::extractFiles, this));
#endif
}

void TarZipUncompressor::extractFiles()
{
    const auto fileInfos = m_tarZipArchive->archiveIndex()->root()->toPreOrderList();

    for(const auto &fileInfo : fileInfos) {
        const auto &absolutePath = fileInfo.absolutePath;

        if(absolutePath.isEmpty()) {
            continue;
        } else if(fileInfo.type == FileNode::Type::Directory) {
            m_targetDir.mkpath(absolutePath);
        } else if(!extractFile(absolutePath, m_targetDir.absoluteFilePath(absolutePath))) {
            break;
        }
    }
}

bool TarZipUncompressor::extractFile(const QString &src, const QString &dst)
{
    // TODO: Write files to disk in chunks
    // Preferably rewrite all tar.gz operations in stream-friendly style
    // with no intermediate files

    QFile file(dst);
    // TODO: check for errors
    if(!file.open(QIODevice::WriteOnly)) {
        setError(BackendError::DiskError, file.errorString());
        return false;
    }

    const auto fileData = m_tarZipArchive->archiveIndex()->fileData(src);
    if(file.write(fileData) != fileData.size()) {
        setError(BackendError::DiskError, file.errorString());
        return false;
    }

    file.close();
    return true;
}
