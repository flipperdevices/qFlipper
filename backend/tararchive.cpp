#include "tararchive.h"

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

#include <QIODevice>
#include <QDateTime>

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

#define BLOCK_SIZE 512
#define CHUNK_SIZE (4 * 1024 * 1024)

struct TarHeader
{
    char name[100];
    char mode[8];
    char owner[8];
    char group[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char unused[255];
};

static_assert(sizeof(TarHeader) == BLOCK_SIZE, "Check TarHeader alignment");

static bool isMemZeros(char *p, size_t len)
{
    while(len--) {
        if(*(p++)) {
            return false;
        }
    }

    return true;
}

static uint32_t calculateChecksum(const TarHeader *header)
{
    uint32_t ret = 256;
    const auto *p = (unsigned char*)header;

    for(size_t i = 0; i < offsetof(TarHeader, checksum); ++i) {
        ret += p[i];
    }

    for(size_t i = offsetof(TarHeader, typeflag); i < sizeof(TarHeader); ++i) {
        ret += p[i];
    }

    return ret;
}

TarArchive::TarArchive(QIODevice *inputFile, QObject *parent):
    QObject(parent),
    m_tarFile(inputFile),
    m_root(new FileNode("", FileNode::Type::Directory))
{
    if(!m_tarFile->open(QIODevice::ReadOnly)) {
        setError(BackendError::DiskError, m_tarFile->errorString());
    } else {
        readTarFile();
    }
}

TarArchive::TarArchive(const QDir &inputDir, QIODevice *outputFile, QObject *parent):
    QObject(parent),
    m_tarFile(outputFile),
    m_root(new FileNode("", FileNode::Type::Directory))
{
    if(!m_tarFile->open(QIODevice::WriteOnly)) {
        setError(BackendError::DiskError, m_tarFile->errorString());
        return;
    }

    auto *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcherBase::finished, this, [=]() {
        watcher->deleteLater();
        emit ready();
    });

#if QT_VERSION < 0x060000
    watcher->setFuture(QtConcurrent::run(this, &TarArchive::assembleTarFile, inputDir));
#else
    watcher->setFuture(QtConcurrent::run(&TarArchive::assembleTarFile, this, inputDir));
#endif
}

FileNode *TarArchive::root() const
{
    return m_root.get();
}

FileNode *TarArchive::file(const QString &fullName)
{
    return m_root->find(fullName);
}

QByteArray TarArchive::fileData(const QString &fullName)
{
    if(!m_tarFile) {
        setError(BackendError::UnknownError, QStringLiteral("Archive file not set"));
        return QByteArray();

    } else if(!m_tarFile->isOpen()) {
        setError(BackendError::UnknownError, QStringLiteral("Archive file is not open"));
        return QByteArray();
    }

    auto *node = file(fullName);
    if(!node) {
        setError(BackendError::UnknownError, QStringLiteral("File not found"));
        return QByteArray();
    }

    if(!node->userData().canConvert<FileInfo>()) {
        setError(BackendError::DataError, QStringLiteral("No valid FileData found in the node."));
        return QByteArray();
    }

    const auto data = node->userData().value<FileInfo>();
    const auto success = m_tarFile->seek(data.offset);

    if(success) {
        return m_tarFile->read(data.size);

    } else {
        setError(BackendError::DiskError, m_tarFile->errorString());
        return QByteArray();
    }
}

void TarArchive::readTarFile()
{
    TarHeader header;
    int emptyCounter = 0;

    do {
        const auto n = m_tarFile->read((char*)&header, sizeof(TarHeader));

        if(n != sizeof(TarHeader)) {
            setError(BackendError::DataError, QStringLiteral("Archive file is truncated"));
            return;

        } else if(isMemZeros((char*)&header, sizeof(TarHeader))) {
            if(++emptyCounter == 2) {
               break;
            } else {
                continue;
            }
        }

        const auto fileSize = strtol(header.size, nullptr, 8);
        const auto fileName = QString(header.name);

        if(header.typeflag == '0') {
            FileInfo data;

            data.offset = m_tarFile->pos();
            data.size = fileSize;

            m_root->addFile(fileName, QVariant::fromValue(data));

        } else if(header.typeflag == '5') {
            m_root->addDirectory(fileName.chopped(1));

        } else {
            setError(BackendError::DataError, QStringLiteral("Only regular files and directories are supported"));
            return;
        }

        // Blocks are always padded to BLOCK_SIZE
        const auto padding = fileSize % BLOCK_SIZE ? BLOCK_SIZE - (fileSize % BLOCK_SIZE) : 0;
        m_tarFile->skip(fileSize + padding);

    } while(m_tarFile->bytesAvailable());
}

void TarArchive::assembleTarFile(const QDir &inputDir)
{
    TarHeader header = {};
    QDirIterator it(inputDir, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        const QFileInfo fileInfo(it.next());
        const auto relativeFilePath = inputDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();

        if(fileInfo.isFile()) {
            snprintf(header.name, sizeof(header.name), "%s", relativeFilePath.data());
            snprintf(header.mode, sizeof(header.mode), "%07o", 0664);
            snprintf(header.size, sizeof(header.size), "%011o", (unsigned int)fileInfo.size());
            header.typeflag = '0';

        } else if(fileInfo.isDir()) {
            snprintf(header.name, sizeof(header.name), "%s/", relativeFilePath.data());
            snprintf(header.mode, sizeof(header.mode), "%07o", 0755);
            snprintf(header.size, sizeof(header.size), "%011o", 0);
            header.typeflag = '5';
        }

        snprintf(header.owner, sizeof(header.owner), "%07o", 1000);
        snprintf(header.group, sizeof(header.group), "%07o", 1000);

        snprintf(header.mtime, sizeof(header.mtime), "%011o", (unsigned int)fileInfo.metadataChangeTime().toSecsSinceEpoch());
        snprintf(header.checksum, sizeof(header.checksum), "%06o", calculateChecksum(&header));

        if(m_tarFile->write((const char*)&header, sizeof(TarHeader)) != sizeof(TarHeader)) {
            setError(BackendError::DiskError, m_tarFile->errorString());
            break;
        }

        if(fileInfo.isFile() && fileInfo.size() > 0) {
            QFile file(fileInfo.absoluteFilePath());

            if(!file.open(QIODevice::ReadOnly)) {
                setError(BackendError::DiskError, file.errorString());
                break;
            }

            qint64 bytesProcessed = 0;

            while(file.bytesAvailable() > 0) {
                const auto chunk = file.read(CHUNK_SIZE);
                if(chunk.isEmpty()) {
                    setError(BackendError::DiskError, QStringLiteral("Failed to read from file: %1").arg(file.errorString()));
                    break;
                }

                if(m_tarFile->write(chunk) != chunk.size()) {
                    setError(BackendError::DiskError, QStringLiteral("Failed to write to file: %1").arg(m_tarFile->errorString()));
                    break;
                }

                bytesProcessed += chunk.size();
            }

            if(bytesProcessed != fileInfo.size()) {
                break;
            }

            // Blocks are always padded to BLOCK_SIZE
            const auto padding = BLOCK_SIZE - (fileInfo.size() % BLOCK_SIZE);
            if(padding) {
                m_tarFile->write(QByteArray(padding, 0));
            }
        }
    }

    m_tarFile->close();
}
