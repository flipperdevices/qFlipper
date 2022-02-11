#pragma once

#include <QByteArray>

#include "messages/flipper.pb.h"

class MessageWrapper
{
public:
    MessageWrapper(const QByteArray &buffer);
    MessageWrapper(MessageWrapper &&other);
    ~MessageWrapper();

    const PB_Main &message() const;
    size_t encodedSize() const;
    bool isComplete() const;

private:
    PB_Main m_message;
    size_t m_encodedSize;
    bool m_isComplete;
};
