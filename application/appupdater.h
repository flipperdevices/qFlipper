#pragma once

#include <QObject>

#include "flipperupdates.h"

class AppUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum class State {
        Idle,
        Downloading,
        Updating,
        ErrorOccured
    };

    Q_ENUM(State)

    AppUpdater(QObject *parent = nullptr);

    State state() const;
    double progress() const;

signals:
    void stateChanged();
    void progressChanged();

public slots:
    bool canUpdate(const Flipper::Updates::VersionInfo &versionInfo);
    void installUpdate(const Flipper::Updates::VersionInfo &versionInfo);

private slots:
    void setState(State state);
    void setProgress(double progress);

private:
    bool performUpdate(const QString &path);

    State m_state;
    double m_progress;
};

