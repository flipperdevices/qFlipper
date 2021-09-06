#pragma once

#include <QMap>
#include <QByteArray>

class QIODevice;

class TarArchive
{
public:
    class FileInfo {
    public:
        enum class Type {
            RegularFile,
            Directory,
            Unknown
        };

        FileInfo();
        FileInfo(const QString name, size_t offset, size_t size, Type type);

        bool isValid() const;

        const QString &name() const;

        size_t offset() const;
        size_t size() const;
        Type type() const;

    private:
        QString m_name;
        size_t m_offset;
        size_t m_size;
        Type m_type;
    };

    TarArchive(QIODevice *file);

     FileInfo fileInfo(const QString &fullName) const;
     QByteArray fileData(const QString &fullName) const;

private:
    void buildIndex();

    QIODevice *m_tarFile;
    QMap<QString, FileInfo> m_fileIndex;
};

