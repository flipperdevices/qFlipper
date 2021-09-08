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
    m_tarFile(file)
{
    if(!m_tarFile->open(QIODevice::ReadOnly)) {
        error_msg("Failed to open the file for reading.");
        return;
    }

    buildIndex();
}

bool TarArchive::isValid() const
{
    return m_tarFile && m_fileIndex.size();
}

QList<TarArchive::FileInfo> TarArchive::files() const
{
    return m_fileIndex.values();
}

TarArchive::FileInfo TarArchive::fileInfo(const QString &fullName) const
{
    return m_fileIndex.value(fullName, FileInfo());
}

QByteArray TarArchive::fileData(const QString &fullName) const
{
    check_return_val(m_tarFile->isOpen(), "Archive not open.", QByteArray());
    const auto fileInfo = m_fileIndex.value(fullName, FileInfo());
    check_return_val(fileInfo.isValid(), "Invalid FileInfo.", QByteArray());

    m_tarFile->seek(fileInfo.offset());
    return m_tarFile->read(fileInfo.size());
}

void TarArchive::buildIndex()
{
    TarHeader header;
    int emptyCounter = 0;

    do {
        const auto n = m_tarFile->read((char*)&header, sizeof(TarHeader));

        if(n != sizeof (TarHeader)) {
            error_msg("Archive file is truncated.");
            return;

        } else if(isMemZeros((char*)&header, sizeof(TarHeader))) {
            if(++emptyCounter == 2) {
               break;
            } else {
                continue;
            }

        } else if(strncmp(header.magic, "ustar", 5)) {
            error_msg("Tar magic constant not found.");
            return;
        }

        const auto fileSize = strtoul(header.size, nullptr, 8);
        const auto fileType = header.typeflag == '0' ? FileInfo::Type::RegularFile :
                              header.typeflag == '5' ? FileInfo::Type::Directory : FileInfo::Type::Unknown;

        FileInfo fileInfo(header.name, m_tarFile->pos(), fileSize, fileType);
        m_fileIndex.insert(fileInfo.name(), fileInfo);

        // Blocks are always padded to BLOCK_SIZE
        const auto padding = fileSize % BLOCK_SIZE ? BLOCK_SIZE - (fileSize % BLOCK_SIZE) : 0;
        m_tarFile->skip(fileSize + padding);

    } while(m_tarFile->bytesAvailable());
}

TarArchive::FileInfo::FileInfo():
    m_offset(0),
    m_size(0),
    m_type(Type::Unknown)
{}

TarArchive::FileInfo::FileInfo(const QString name, size_t offset, size_t size, Type type):
    m_name(name),
    m_offset(offset),
    m_size(size),
    m_type(type)
{}

bool TarArchive::FileInfo::isValid() const
{
    return !m_name.isEmpty() || m_type != Type::Unknown;
}

const QString &TarArchive::FileInfo::name() const
{
    return m_name;
}

size_t TarArchive::FileInfo::offset() const
{
    return m_offset;
}

size_t TarArchive::FileInfo::size() const
{
    return m_size;
}

TarArchive::FileInfo::Type TarArchive::FileInfo::type() const
{
    return m_type;
}
