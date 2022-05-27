#include "tarzipuncompressor.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#include "tararchive.h"
#include "tarziparchive.h"
#include "tempdirectories.h"

TarZipUncompressor::TarZipUncompressor(QFile *tarZipFile, const QDir &targetDir, QObject *parent):
    QObject(parent),
    m_archive(new TarZipArchive(tarZipFile, this)),
    m_targetDir(targetDir)
{
    connect(m_archive, &TarZipArchive::ready, this, &TarZipUncompressor::onArchiveReady);
}

TarZipUncompressor::~TarZipUncompressor()
{}

void TarZipUncompressor::onArchiveReady()
{
    auto *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, &TarZipUncompressor::finished);
    watcher->setFuture(QtConcurrent::run(this, &TarZipUncompressor::extractFiles));
}

void TarZipUncompressor::extractFiles()
{
    const auto fileInfos = m_archive->archiveIndex()->root()->toPreOrderList();

    for(const auto &fileInfo : fileInfos) {
        const auto &absolutePath = fileInfo.absolutePath;

        if(absolutePath.isEmpty()) {
            continue;
        } else if(fileInfo.type == FileNode::Type::Directory) {
            m_targetDir.mkpath(absolutePath);
        } else {
            extractFile(absolutePath, m_targetDir.absoluteFilePath(absolutePath));
        }
    }
}

void TarZipUncompressor::extractFile(const QString &src, const QString &dst)
{
    // TODO: Write files to disk in chunks
    // Preferably rewrite all tar.gz operations in stream-friendly style
    // with no intermediate files

    QFile file(dst);
    // TODO: check for errors
    file.open(QIODevice::WriteOnly);
    file.write(m_archive->archiveIndex()->fileData(src));
    file.close();
}
