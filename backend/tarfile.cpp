#include "tarfile.h"

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

TarFile::TarFile(QIODevice *file):
    m_file(file)
{
    if(!m_file->open(QIODevice::ReadOnly)) {
        error_msg("Failed to open the file for reading.");
        return;
    }

    buildIndex();
}

void TarFile::buildIndex()
{
    TarHeader header;
    int emptyCounter = 0;

    do {
        const auto n = m_file->read((char*)&header, sizeof(TarHeader));

        if(n != sizeof (TarHeader)) {
            error_msg("Corrupted file header");
            return;

        } else if(isMemZeros((char*)&header, sizeof(TarHeader))) {
            if(++emptyCounter == 2) {
               break;
            } else {
                continue;
            }
        }

        const auto fileSize = strtoul(header.size, nullptr, 8);

        qDebug() << "File name:" << header.name;
        qDebug() << "File size:" << fileSize;
        qDebug() << "File type:" << header.typeflag;
        qDebug() << "Magic    :" << header.magic << Qt::endl;

        const auto padding = fileSize % BLOCK_SIZE ? BLOCK_SIZE - (fileSize % BLOCK_SIZE) : 0;
        m_file->skip(fileSize + padding);

    } while(m_file->bytesAvailable());
}
