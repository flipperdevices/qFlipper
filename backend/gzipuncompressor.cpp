#include "gzipuncompressor.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <QIODevice>

#include <zlib.h>

#include "macros.h"

#define CHUNK_SIZE 1024

GZipUncompressor::GZipUncompressor(QIODevice *in, QIODevice *out, QObject *parent):
    QObject(parent),
    m_in(in),
    m_out(out),
    m_isError(false),
    m_errorString(QStringLiteral("No Error")),
    m_progress(0)
{
    auto *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        info_msg(QStringLiteral("Uncompression finished :%1.").arg(errorString()));

        m_in->close();
        m_out->close();

        m_isError = watcher->result();

        watcher->deleteLater();
        emit finished();
    });

    watcher->setFuture(QtConcurrent::run(this, &GZipUncompressor::uncompress));
}

GZipUncompressor::~GZipUncompressor()
{
    m_in->close();
    m_out->close();
}

bool GZipUncompressor::isError() const
{
    return m_isError;
}

const QString &GZipUncompressor::errorString()
{
    return m_errorString;
}

double GZipUncompressor::progress() const
{
    return m_progress;
}

void GZipUncompressor::setError(const QString &errorString)
{
    m_errorString = errorString;
}

void GZipUncompressor::setProgress(double progress)
{
    if(qFuzzyCompare(m_progress, progress)) {
        return;
    }

    m_progress = progress;
    emit progressChanged();
}

bool GZipUncompressor::uncompress()
{
    if(!m_in->open(QIODevice::ReadOnly)) {
        setError(QStringLiteral("Failed to open the input file for reading"));
        return false;

    } else if(m_in->bytesAvailable() <= 4) {
        setError(QStringLiteral("The input file is empty"));
        return false;
    } else if(!m_out->open(QIODevice::WriteOnly)) {
        setError(QStringLiteral("Failed to open the output file for writing"));
        return false;
    }

    const auto totalSize = m_in->bytesAvailable();
    info_msg(QStringLiteral("Uncompressing file with size of %1 bytes...").arg(totalSize));

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    const auto err = inflateInit2(&stream, 15 + 16);
    if(err != Z_OK) {
        setError(QStringLiteral("Failed to initialise deflate method"));
        return false;
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
                setError(QStringLiteral("Error during uncompression"));
                return false;
            }

            m_out->write(outbuf, CHUNK_SIZE - stream.avail_out);

        } while(!stream.avail_out);

        setProgress(progress() + (100.0 * n) / totalSize);

    } while(m_in->bytesAvailable());

    inflateEnd(&stream);

    m_in->close();
    m_out->close();

    return false;
}
