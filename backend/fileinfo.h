#pragma once

#include <QByteArray>

enum class FileType {
    Directory,
    RegularFile,
    Storage,
    Unknown
};

struct FileInfo {
    QByteArray name;
    QByteArray absolutePath;
    FileType type;
    qint64 size;
};
