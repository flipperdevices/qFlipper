#pragma once

#include <QObject>

#include "failable.h"

class QFile;
class TarArchive;

class TarZipArchive : public QObject, public Failable
{
    Q_OBJECT

public:
    TarZipArchive(QFile *tarZipFile, QObject *parent = nullptr);
    ~TarZipArchive();

    TarArchive *archiveIndex() const;

signals:
    void ready();

private:
    QFile *m_tarFile;
    TarArchive *m_archiveIndex;
};

