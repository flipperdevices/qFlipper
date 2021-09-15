#pragma once

#include <QObject>

class QIODevice;

class GZipUncompressor : public QObject
{
    Q_OBJECT

public:
    GZipUncompressor(QIODevice *in, QIODevice* out, QObject *parent = nullptr);
    ~GZipUncompressor();

    bool isError() const;
    const QString &errorString();
    double progress() const;

signals:
    void finished();
    void progressChanged();

private:
    void setError(const QString &errorString);
    void setProgress(double progress);

    bool uncompress();

    QIODevice *m_in;
    QIODevice *m_out;

    bool m_isError;
    QString m_errorString;
    double m_progress;
};


