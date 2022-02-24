#include "getfiletreeoperation.h"

#include "flipperzero/devicestate.h"
#include "flipperzero/protobufsession.h"
#include "flipperzero/rpc/storagelistoperation.h"

using namespace Flipper;
using namespace Zero;

GetFileTreeOperation::GetFileTreeOperation(ProtobufSession *rpc, DeviceState *deviceState, const QByteArray &rootPath, QObject *parent):
    AbstractUtilityOperation(rpc, deviceState, parent),
    m_rootPath(rootPath),
    m_pendingCount(0)
{}

const QString GetFileTreeOperation::description() const
{
    return QStringLiteral("Get File Tree @%1").arg(deviceState()->name());
}

const FileInfoList &GetFileTreeOperation::files() const
{
    return m_result;
}

void GetFileTreeOperation::nextStateLogic()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::Running);
        listDirectory(m_rootPath);

    } else if(operationState() == State::Running) {
        --m_pendingCount;
        auto *op = qobject_cast<StorageListOperation*>(sender());

        if(op->isError()) {
            finishWithError(op->error(), op->errorString());
            return;
        }

        for(const auto &fileInfo : qAsConst(op->files())) {
            if(fileInfo.type == FileType::Directory) {
                listDirectory(fileInfo.absolutePath);
            }

            m_result.push_back(fileInfo);
        }

        if(!m_pendingCount) {
            finish();
        }
    }
}

void GetFileTreeOperation::listDirectory(const QByteArray &path)
{
    ++m_pendingCount;
    auto *op = rpc()->storageList(path);
    connect(op, &AbstractOperation::finished, this, &GetFileTreeOperation::nextStateLogic);
}
