#pragma once

#include <QMap>
#include <QObject>
#include <QByteArray>
#include <QFileInfoList>
#include <QSharedPointer>

#include "filenode.h"
#include "failable.h"

class QDir;
class QIODevice;

class TarArchive : public QObject, public Failable
{
    Q_OBJECT

public:
    struct FileInfo {
        qint64 offset;
        qint64 size;
    };

    TarArchive(QIODevice *inputFile, QObject *parent = nullptr);
    TarArchive(const QDir &inputDir, QIODevice *outputFile, QObject *parent = nullptr);

    FileNode *root() const;
    FileNode *file(const QString &fullName);
    QByteArray fileData(const QString &fullName);

signals:
    void ready();

private:
    void readTarFile();
    void assembleTarFile(const QDir &inputDir);

    QIODevice *m_tarFile;
    QSharedPointer<FileNode> m_root;
};

Q_DECLARE_METATYPE(TarArchive::FileInfo)
