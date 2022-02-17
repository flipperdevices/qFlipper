#include "abstractprotobufoperation.h"

#include "mainresponseinterface.h"

using namespace Flipper;
using namespace Zero;

AbstractProtobufOperation::AbstractProtobufOperation(uint32_t id, QObject *parent):
    AbstractOperation(parent),
    m_id(id)
{}

AbstractProtobufOperation::~AbstractProtobufOperation()
{}

uint32_t AbstractProtobufOperation::id() const
{
    return m_id;
}

bool AbstractProtobufOperation::hasNext() const
{
    // Default implementation for single-part operations
    return false;
}

bool AbstractProtobufOperation::isFinished() const
{
    return operationState() == AbstractOperation::Finished;
}

void AbstractProtobufOperation::start()
{
    // TODO: Decide whether to use this method at all
}

void AbstractProtobufOperation::feedResponse(QObject *response)
{
    if(!processResponse(response)) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Failed to process protobuf response"));
    } else if(!(qobject_cast<MainResponseInterface*>(response)->hasNext())) {
        finish();
    }
}
