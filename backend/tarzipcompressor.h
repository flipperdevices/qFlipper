#pragma once

#include <QDir>
#include <QUrl>
#include <QObject>

#include "failable.h"

class TarZipCompressor : public QObject, public Failable
{
    Q_OBJECT

public:
    TarZipCompressor(const QDir &inputDir, const QUrl &outputFileUrl, QObject *parent = nullptr);

signals:
    void finished();
};
