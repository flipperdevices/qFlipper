#include "tarziparchive.h"

#include <QDir>
#include <QFile>

#include "tararchive.h"
#include "gzipuncompressor.h"

TarZipArchive::TarZipArchive(QFile *tarZipFile, QObject *parent):
    QObject(parent),
    m_archiveIndex(nullptr)
{
    const QFileInfo tzFileInfo(*tarZipFile);
    const auto tzFileDir = tzFileInfo.absoluteDir();
    const auto fileName = tzFileInfo.baseName() + QStringLiteral(".tar");

    m_tarFile = new QFile(tzFileDir.absoluteFilePath(fileName), this);

    auto *uncompressor = new GZipUncompressor(tarZipFile, m_tarFile, this);

    if(uncompressor->isError()) {
        setErrorString(QStringLiteral("Failed to uncompress *tar.gz file: %1").arg(uncompressor->errorString()));
        return;
    }

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            setErrorString(QStringLiteral("Failed to uncompress *tar.gz file: %1").arg(uncompressor->errorString()));
        } else {
            m_archiveIndex = new TarArchive(m_tarFile);

            if(m_archiveIndex->isError()) {
                setErrorString(QStringLiteral("Failed to build archive index: %1").arg(m_archiveIndex->errorString()));
            }
        }

        emit ready();
    });
}

TarZipArchive::~TarZipArchive()
{
    if(m_archiveIndex) {
        delete m_archiveIndex;
    }

    m_tarFile->remove();
}

TarArchive *TarZipArchive::archiveIndex() const
{
    return m_archiveIndex;
}
