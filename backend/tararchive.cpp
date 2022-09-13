#include "tararchive.h"

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QIODevice>
#include <QDebug>

#define BLOCK_SIZE 512

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
    char magic[6];
    char unused4[249];
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
        readFile();
    }
}

TarArchive::TarArchive(const QDir &inputDir, QIODevice *outputFile, QObject *parent):
    QObject(parent),
    m_tarFile(outputFile),
    m_root(new FileNode("", FileNode::Type::Directory))
{
    if(!m_tarFile->open(QIODevice::WriteOnly)) {
        setError(BackendError::DiskError, m_tarFile->errorString());
    } else {
        writeFile(inputDir);
    }
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

void TarArchive::readFile()
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

        } else if(strncmp(header.magic, "ustar", 5)) {
            setError(BackendError::DataError, QStringLiteral("Tar magic constant not found."));
            return;
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

void TarArchive::writeFile(const QDir &inputDir)
{
    TarHeader header = {};
    snprintf(header.magic, sizeof(header.magic), "%s", "ustar");

    QDirIterator it(inputDir, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        const QFileInfo fileInfo(it.next());
        const auto relativeFilePath = inputDir.relativeFilePath(fileInfo.absoluteFilePath()).toLocal8Bit();

        if(fileInfo.isFile()) {
            snprintf(header.name, sizeof(header.name), "%s", relativeFilePath.data());
            snprintf(header.mode, sizeof(header.mode), "%o", 0664);
            snprintf(header.size, sizeof(header.size), "%o", (unsigned int)fileInfo.size());
            header.typeflag = '0';

        } else if(fileInfo.isDir()) {
            snprintf(header.name, sizeof(header.name), "%s/", relativeFilePath.data());
            snprintf(header.mode, sizeof(header.mode), "%o", 0775);
            snprintf(header.size, sizeof(header.size), "0");
            header.typeflag = '5';
        }

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

            const auto fileData = file.readAll();
            if(fileData.size() != fileInfo.size()) {
                setError(BackendError::DiskError, file.errorString());
                break;
            }

            if(m_tarFile->write(fileData) != fileData.size()) {
                setError(BackendError::DiskError, m_tarFile->errorString());
                break;
            }

            const auto paddingSize = BLOCK_SIZE - (fileInfo.size() % BLOCK_SIZE);
            if(paddingSize) {
                m_tarFile->write(QByteArray(paddingSize, 0));
            }
        }
    }

    m_tarFile->close();
}
