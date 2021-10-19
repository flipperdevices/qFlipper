#include "getfiletreeoperation.h"

#include <QTimer>

#include "flipperzero/devicestate.h"
#include "flipperzero/commandinterface.h"
#include "flipperzero/cli/listoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

GetFileTreeOperation::GetFileTreeOperation(CommandInterface *cli, DeviceState *deviceState, const QByteArray &rootPath, QObject *parent):
    AbstractUtilityOperation(cli, deviceState, parent),
    m_rootPath(rootPath),
    m_pendingCount(0)
{}

const QString GetFileTreeOperation::description() const
{
    return QStringLiteral("Get File Tree @%1").arg(deviceState()->name());
}

const FileInfoList &GetFileTreeOperation::result() const
{
    return m_result;
}

void GetFileTreeOperation::advanceOperationState()
{
    if(operationState() == BasicOperationState::Ready) {
        setOperationState(State::Running);
        listDirectory(m_rootPath);

    } else if(operationState() == State::Running) {
        --m_pendingCount;
        auto *op = qobject_cast<ListOperation*>(sender());

        if(op->isError()) {
            finishWithError(op->errorString());
            return;
        }

        for(const auto &fileInfo : qAsConst(op->result())) {
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
    auto *op = cli()->list(path);
    connect(op, &AbstractOperation::finished, this, &GetFileTreeOperation::advanceOperationState);
}
