#pragma once

#include <QDir>
#include <QObject>

#include "failable.h"

class QFile;
class TarZipArchive;

class TarZipUncompressor : public QObject, public Failable
{
    Q_OBJECT

public:
    TarZipUncompressor(QFile *tarZipFile, const QDir &targetDir, QObject *parent = nullptr);
    virtual ~TarZipUncompressor();

signals:
    void progressChanged();
    void finished();

private slots:
    void onArchiveReady();

private:
    void extractFiles();
    void extractFile(const QString &src, const QString &dst);

    TarZipArchive *m_archive;
    QDir m_targetDir;
};

