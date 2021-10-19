#pragma once

#include <QObject>

#include "failable.h"

class QIODevice;

class GZipUncompressor : public QObject, public Failable
{
    Q_OBJECT

public:
    GZipUncompressor(QIODevice *in, QIODevice* out, QObject *parent = nullptr);
    ~GZipUncompressor();

    double progress() const;

signals:
    void finished();
    void progressChanged();

private:
    void setProgress(double progress);
    void doUncompress();
    void closeFiles();

    QIODevice *m_in;
    QIODevice *m_out;

    double m_progress;
};


