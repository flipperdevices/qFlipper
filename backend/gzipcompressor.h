#pragma once

#include <QObject>

#include "failable.h"

class QIODevice;

class GZipCompressor : public QObject, public Failable
{
    Q_OBJECT

public:
    GZipCompressor(QIODevice *in, QIODevice *out, QObject *parent = nullptr);

    double progress() const;

signals:
    void finished();
    void progressChanged();

private:
    void setProgress(double progress);
    void doCompress();
    void closeFiles();

    QIODevice *m_in;
    QIODevice *m_out;

    double m_progress;
};

