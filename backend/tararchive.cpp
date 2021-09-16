#include "tararchive.h"

#include <QIODevice>

#include "macros.h"

#define BLOCK_SIZE 512

struct TarHeader
{
  char name[100];
  char unused1[24];
  char size[12];
  char unused2[20];
  char typeflag;
  char unused3[100];
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

TarArchive::TarArchive():
    m_tarFile(nullptr)
{}

TarArchive::TarArchive(QIODevice *file):
    m_tarFile(file),
    m_root(new FileNode("", FileNode::Type::Directory))
{
    buildIndex();
}

FileNode *TarArchive::file(const QString &fullName)
{
    return m_root->find(fullName);
}

QByteArray TarArchive::fileData(const QString &fullName)
{
    if(!m_tarFile) {
        setError(QStringLiteral("Archive file not set"));
        return QByteArray();
    }

    auto *node = file(fullName);

    if(!node->userData().canConvert<FileInfo>()) {
        setError(QStringLiteral("No valid FileData found in the node."));
        return QByteArray();
    }

    const auto data = node->userData().value<FileInfo>();
    const auto success = m_tarFile->seek(data.offset);

    if(success) {
        return m_tarFile->read(data.size);

    } else {
        setError(m_tarFile->errorString());
        return QByteArray();
    }
}

void TarArchive::buildIndex()
{
    TarHeader header;
    int emptyCounter = 0;

    do {
        const auto n = m_tarFile->read((char*)&header, sizeof(TarHeader));

        if(n != sizeof(TarHeader)) {
            setError(QStringLiteral("Archive file is truncated"));
            return;

        } else if(isMemZeros((char*)&header, sizeof(TarHeader))) {
            if(++emptyCounter == 2) {
               break;
            } else {
                continue;
            }

        } else if(strncmp(header.magic, "ustar", 5)) {
            setError(QStringLiteral("Tar magic constant not found."));
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
            setError(QStringLiteral("Only regular files and directories are supported"));
            return;
        }

        // Blocks are always padded to BLOCK_SIZE
        const auto padding = fileSize % BLOCK_SIZE ? BLOCK_SIZE - (fileSize % BLOCK_SIZE) : 0;
        m_tarFile->skip(fileSize + padding);

    } while(m_tarFile->bytesAvailable());
}
