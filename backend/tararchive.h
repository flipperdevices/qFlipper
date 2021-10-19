#pragma once

#include <QMap>
#include <QByteArray>
#include <QSharedPointer>

#include "filenode.h"
#include "failable.h"

class QIODevice;

class TarArchive : public Failable
{
public:
    struct FileInfo {
        qint64 offset;
        qint64 size;
    };

    TarArchive();
    TarArchive(QIODevice *file);
    virtual ~TarArchive();

    FileNode *file(const QString &fullName);
    QByteArray fileData(const QString &fullName);

private:
    void buildIndex();

    QIODevice *m_tarFile;
    QSharedPointer<FileNode> m_root;
};

Q_DECLARE_METATYPE(TarArchive::FileInfo);
