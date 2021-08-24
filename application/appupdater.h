#pragma once

#include <QObject>

#include "flipperupdates.h"

class AppUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    AppUpdater(QObject *parent = nullptr);

    double progress() const;

signals:
    void progressChanged();
    void downloadFinished();
    void errorOccured();

public slots:
    void installUpdate(const Flipper::Updates::VersionInfo &versionInfo);

private slots:
    void setProgress(double progress);

private:
    bool performUpdate(const QString &path);

    double m_progress;
};

