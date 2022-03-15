#include "abstractprotobufoperation.h"

#include <QTimer>

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

bool AbstractProtobufOperation::hasMoreData() const
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
    if(!begin()) {
        // begin() has to fill the error() and errorString() fields
        finishLater();
    } else {
        setOperationState(Started);
        startTimeout();
    }
}

void AbstractProtobufOperation::finishLater()
{
    QTimer::singleShot(0, this, &AbstractOperation::finish);
}

void AbstractProtobufOperation::abort(const QString &reason)
{
    finishWithError(BackendError::UnknownError, reason);
}

void AbstractProtobufOperation::feedResponse(QObject *response)
{
    auto *mainResponse = qobject_cast<MainResponseInterface*>(response);

    if(mainResponse->isError()) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Device replied with error: %1").arg(mainResponse->errorString()));
    } else if(!processResponse(response)) {
        finishWithError(BackendError::ProtocolError, QStringLiteral("Operation finished with error: %1").arg(mainResponse->errorString()));
    } else if(!mainResponse->hasNext()) {
        finish();
    } else {
        startTimeout();
    }
}

bool AbstractProtobufOperation::begin()
{
    // Empty default implementation
    return true;
}

bool AbstractProtobufOperation::processResponse(QObject *response)
{
    // Default implementation, checks whether we got an empty response
    // TODO: add isOk() check
    return qobject_cast<EmptyResponseInterface*>(response);
}
