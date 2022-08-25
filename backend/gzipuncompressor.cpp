#include "gzipuncompressor.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QLoggingCategory>
#include <QFutureWatcher>
#include <QIODevice>
#include <QDebug>

#include <zlib.h>

Q_LOGGING_CATEGORY(LOG_UNZIP, "ZIP")

#define CHUNK_SIZE 1024

GZipUncompressor::GZipUncompressor(QIODevice *in, QIODevice *out, QObject *parent):
    QObject(parent),
    m_in(in),
    m_out(out),
    m_progress(0)
{
    if(!m_in->open(QIODevice::ReadOnly)) {
        setError(BackendError::DiskError, m_in->errorString());
        return;

    } else if(!m_out->open(QIODevice::WriteOnly)) {
        setError(BackendError::DiskError, m_out->errorString());
        return;
    }

    auto *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        qCDebug(LOG_UNZIP).noquote() << "Uncompression finished :" << errorString();
        watcher->deleteLater();
        emit finished();
    });
#if QT_VERSION < 0x060000
    watcher->setFuture(QtConcurrent::run(this, &GZipUncompressor::doUncompress));
#else
    watcher->setFuture(QtConcurrent::run(&GZipUncompressor::doUncompress, this));
#endif
}

GZipUncompressor::~GZipUncompressor()
{}

double GZipUncompressor::progress() const
{
    return m_progress;
}

void GZipUncompressor::setProgress(double progress)
{
    if(qFuzzyCompare(m_progress, progress)) {
        return;
    }

    m_progress = progress;
    emit progressChanged();
}

void GZipUncompressor::doUncompress()
{
    if(m_in->bytesAvailable() <= 4) {
        setError(BackendError::DataError, QStringLiteral("The input file is empty"));
        return;
    }

    const auto totalSize = m_in->bytesAvailable();
    qCDebug(LOG_UNZIP) << "Uncompressing file with size of" << totalSize << "bytes...";

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    const auto err = inflateInit2(&stream, 15 + 16);
    if(err != Z_OK) {
        setError(BackendError::UnknownError, QStringLiteral("Failed to initialise deflate method"));
        return;
    }

    char inbuf[CHUNK_SIZE];
    char outbuf[CHUNK_SIZE];

    do {
        const auto n = m_in->read(inbuf, CHUNK_SIZE);
        stream.avail_in = n;
        stream.next_in = (Bytef*)inbuf;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = (Bytef*)outbuf;

            const auto err = inflate(&stream, Z_NO_FLUSH);
            const auto errorOccured = (err == Z_MEM_ERROR) || (err == Z_DATA_ERROR) || (err == Z_NEED_DICT);

            if(errorOccured) {
                inflateEnd(&stream);
                setError(BackendError::DataError, QStringLiteral("Error during uncompression"));
                return;
            }

            m_out->write(outbuf, CHUNK_SIZE - stream.avail_out);

        } while(!stream.avail_out);

        setProgress(progress() + (100.0 * n) / totalSize);

    } while(m_in->bytesAvailable());

    inflateEnd(&stream);
    closeFiles();
}

void GZipUncompressor::closeFiles()
{
    m_in->close();
    m_out->close();
}
