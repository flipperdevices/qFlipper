#pragma once

#include <QObject>

#include "flipperupdates.h"

class ApplicationUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum State {
        Idle,
        Downloading,
        Updating,
        ErrorOccured
    };

    Q_ENUM(State)

    ApplicationUpdater(QObject *parent = nullptr);

    void reset();

    State state() const;
    double progress() const;

    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo) const;
    void installUpdate(const Flipper::Updates::VersionInfo &versionInfo);

signals:
    void stateChanged();
    void progressChanged();

private slots:
    void setState(ApplicationUpdater::State state);
    void setProgress(double progress);

private:
    bool performUpdate(const QString &path);

    State m_state;
    double m_progress;
};

