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

signals:
    void finished();

private slots:
    void onArchiveReady();

private:
    void extractFiles();
    bool extractFile(const QString &src, const QString &dst);

    TarZipArchive *m_tarZipArchive;
    QDir m_targetDir;
};

