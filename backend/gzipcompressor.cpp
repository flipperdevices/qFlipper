#include "gzipcompressor.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QLoggingCategory>
#include <QFutureWatcher>
#include <QIODevice>
#include <QDebug>

#include <zlib.h>

Q_DECLARE_LOGGING_CATEGORY(LOG_UNZIP)

#define CHUNK_SIZE 1024

GZipCompressor::GZipCompressor(QIODevice *in, QIODevice *out, QObject *parent):
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
        qCDebug(LOG_UNZIP).noquote() << "Compression finished:" << errorString();
        watcher->deleteLater();
        emit finished();
    });
#if QT_VERSION < 0x060000
    watcher->setFuture(QtConcurrent::run(this, &GZipCompressor::doCompress));
#else
    watcher->setFuture(QtConcurrent::run(&GZipCompressor::doCompress, this));
#endif
}

double GZipCompressor::progress() const
{
    return m_progress;
}

void GZipCompressor::setProgress(double progress)
{
    if(qFuzzyCompare(m_progress, progress)) {
        return;
    }

    m_progress = progress;
    emit progressChanged();
}

void GZipCompressor::doCompress()
{
    const auto totalSize = m_in->bytesAvailable();

    if(!totalSize) {
        setError(BackendError::DataError, QStringLiteral("The input file is empty"));
        return;
    }

    qCDebug(LOG_UNZIP) << "Compressing file with size of" << totalSize << "bytes...";

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    const auto err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
    if(err != Z_OK) {
        setError(BackendError::UnknownError, QStringLiteral("Failed to initialise deflate method"));
        return;
    }

    char inbuf[CHUNK_SIZE];
    char outbuf[CHUNK_SIZE];
    int flushMode;

    do {
        const auto n = m_in->read(inbuf, CHUNK_SIZE);
        stream.avail_in = n;
        stream.next_in = (Bytef*)inbuf;
        flushMode = m_in->bytesAvailable() ? Z_NO_FLUSH : Z_FINISH;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = (Bytef*)outbuf;

            const auto err = deflate(&stream, flushMode);

            if(err == Z_STREAM_ERROR) {
                deflateEnd(&stream);
                setError(BackendError::DataError, QStringLiteral("Error during compression"));
                return;
            }

            m_out->write(outbuf, CHUNK_SIZE - stream.avail_out);

        } while(!stream.avail_out);

    } while(flushMode != Z_FINISH);

    deflateEnd(&stream);
    closeFiles();
}

void GZipCompressor::closeFiles()
{
    m_in->close();
    m_out->close();
}
