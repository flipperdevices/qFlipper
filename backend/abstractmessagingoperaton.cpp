#include "abstractmessagingoperaton.h"

AbstractMessagingOperaton::AbstractMessagingOperaton(QObject *parent):
    AbstractOperation(parent),
    m_progress(0)
{}

const QString &AbstractMessagingOperaton::message() const
{
    return m_message;
}

void AbstractMessagingOperaton::setMessage(const QString &newMessage)
{
    if(m_message == newMessage) {
        return;
    }

    m_message = newMessage;
    emit messageChanged();
}

double AbstractMessagingOperaton::progress() const
{
    return m_progress;
}

void AbstractMessagingOperaton::setProgress(double newProgress)
{
    if(qFuzzyCompare(m_progress, newProgress)) {
        return;
    }

    m_progress = newProgress;
    emit progressChanged();
}
