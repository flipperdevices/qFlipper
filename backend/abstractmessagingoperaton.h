#pragma once

#include "abstractoperation.h"

class AbstractMessagingOperaton : public AbstractOperation
{
    Q_OBJECT

public:
    AbstractMessagingOperaton(QObject *parent = nullptr);

    const QString &message() const;
    void setMessage(const QString &newMessage);

    double progress() const;
    void setProgress(double newProgress);

signals:
    void messageChanged();
    void progressChanged();

private:
    QString m_message;
    bool m_progress;
};

