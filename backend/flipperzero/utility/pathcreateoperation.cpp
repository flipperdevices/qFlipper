#include "pathcreateoperation.h"

#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagestatoperation.h"
#include "flipperzero/rpc/storagemkdiroperation.h"

using namespace Flipper;
using namespace Zero;

PathCreateOperation::PathCreateOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_remotePath(remotePath),
    m_currentElementIndex(-1),
    m_lastExistingElementIndex(-1)
{}

const QString PathCreateOperation::description() const
{
    return QStringLiteral("Create Path @%1").arg(QString(m_remotePath));
}

bool PathCreateOperation::pathExists() const
{
    return m_lastExistingElementIndex == m_remotePathElements.size();
}

void PathCreateOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(CheckingRootElement);
        checkRootElement();

    } else if(operationState() == CheckingRootElement) {
        setOperationState(CheckingRemotePath);
        checkRemotePath();

    } else if(operationState() == CheckingRemotePath) {
        setOperationState(CreatingRemotePath);
        createRemotePath();

    } else if(operationState() == CreatingRemotePath) {
        finish();

    } else {
        finishWithError(BackendError::UnknownError, QStringLiteral("Unexpected state"));
    }
}

void PathCreateOperation::checkRootElement()
{
    if(!m_remotePath.startsWith('/')) {
        finishWithError(BackendError::UnknownError, QStringLiteral("Path must start with the slash ('/') symbol"));
        return;
    }

    m_remotePathElements = m_remotePath.right(m_remotePath.size() - 1).split('/');

    if(m_remotePathElements.isEmpty()) {
        finishWithError(BackendError::UnknownError, QStringLiteral("Path is empty"));
        return;
    }

    const auto &rootElement = m_remotePathElements.first();

    if((rootElement != QByteArrayLiteral("ext")) && (rootElement != ("int"))) {
        finishWithError(BackendError::UnknownError, QStringLiteral("Invalid remote path: must start with \"int\" or \"ext\""));
    } else if(m_remotePathElements.size() == 1) {
        finish();
    } else {
        m_lastExistingElementIndex = 1;
        advanceOperationState();
    }
}

void PathCreateOperation::checkRemotePath()
{
    const auto currentPath = '/' + m_remotePathElements.mid(0, m_lastExistingElementIndex + 1).join('/');

    auto *operation = rpc()->storageStat(currentPath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
            return;

        } else if(operation->type() == StorageStatOperation::RegularFile) {
            finishWithError(BackendError::UnknownError, QStringLiteral("Remote path %1 is a regular file").arg(QString(currentPath)));
            return;

        } else if(operation->type() == StorageStatOperation::Directory) {
            if(++m_lastExistingElementIndex == m_remotePathElements.size()) {
                finish();
            } else {
                checkRemotePath();
            }

        } else {
            m_currentElementIndex = m_lastExistingElementIndex;
            advanceOperationState();
        }
    });
}

void PathCreateOperation::createRemotePath()
{
    const auto currentPath = '/' + m_remotePathElements.mid(0, m_currentElementIndex + 1).join('/');

    auto *operation = rpc()->storageMkdir(currentPath);

    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else if(++m_currentElementIndex == m_remotePathElements.size()) {
            advanceOperationState();
        } else {
            createRemotePath();
        }
    });
}
