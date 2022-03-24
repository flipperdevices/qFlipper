#include "directorylistoperation.h"

#include "flipperzero/flipperzero.h"
#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"

#include "flipperzero/rpc/storageinfooperation.h"
#include "flipperzero/rpc/storagelistoperation.h"

using namespace Flipper;
using namespace Zero;

DirectoryListOperation::DirectoryListOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &remotePath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_remotePath(remotePath),
    m_isSDCardPresent(false)
{}

const QString DirectoryListOperation::description() const
{
    return QStringLiteral("List Directory @%1").arg(QString(m_remotePath));
}

bool DirectoryListOperation::isRoot() const
{
    return m_remotePath == QByteArrayLiteral("/");
}

bool DirectoryListOperation::isSDCardPath() const
{
    return m_remotePath.startsWith(QByteArrayLiteral("/ext"));
}

bool DirectoryListOperation::isSDCardPresent() const
{
    return m_isSDCardPresent;
}

const FileInfoList &DirectoryListOperation::files() const
{
    return m_files;
}

void DirectoryListOperation::nextStateLogic()
{
    if(operationState() == Ready) {
        setOperationState(CheckingSDCard);
        checkSDCard();

    } else if(operationState() == CheckingSDCard) {
        setOperationState(ListingDirectory);
        listDirectory();

    } else if(operationState() == ListingDirectory) {
        finish();
    }
}

void DirectoryListOperation::checkSDCard()
{
    if(!isRoot() && !isSDCardPath()) {
        advanceOperationState();
        return;
    }

    auto *operation = rpc()->storageInfo(QByteArrayLiteral("/ext"));
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            m_isSDCardPresent = operation->isPresent();

            if(!m_isSDCardPresent && isSDCardPath()) {
                setOperationState(ListingDirectory);
            }

            advanceOperationState();
        }
    });
}

void DirectoryListOperation::listDirectory()
{
    auto *operation = rpc()->storageList(m_remotePath);
    connect(operation, &AbstractOperation::finished, this, [=]() {
        if(operation->isError()) {
            finishWithError(operation->error(), operation->errorString());
        } else {
            m_files = operation->files();
            advanceOperationState();
        }
    });
}
