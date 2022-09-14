#include "tarzipcompressor.h"

#include <QFile>

#include "tarziparchive.h"

TarZipCompressor::TarZipCompressor(const QDir &inputDir, const QUrl &outputFileUrl, QObject *parent):
    QObject(parent)
{
    auto *outputFile = new QFile(outputFileUrl.toLocalFile(), this);
    auto *tarZipArchive = new TarZipArchive(inputDir, outputFile, this);

    if(tarZipArchive->isError()) {
        setError(tarZipArchive->error(), tarZipArchive->errorString());
        return;
    }

    connect(tarZipArchive, &TarZipArchive::ready, this, [=]() {
        if(tarZipArchive->isError()) {
            setError(tarZipArchive->error(), tarZipArchive->errorString());
        }
        emit finished();
    });
}
