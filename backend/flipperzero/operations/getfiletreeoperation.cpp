#include "getfiletreeoperation.h"

#include <QTimer>

#include "flipperzero/flipperzero.h"
#include "flipperzero/storagecontroller.h"
#include "flipperzero/storage/listoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

GetFileTreeOperation::GetFileTreeOperation(FlipperZero *device, const QByteArray &rootPath, QObject *parent):
    Operation(device, parent),
    m_rootPath(rootPath)
{}

const QString GetFileTreeOperation::description() const
{
    return QStringLiteral("Get File Tree @%1").arg(QString(m_rootPath));
}

const GetFileTreeOperation::FileInfoList &GetFileTreeOperation::result() const
{
    return m_result;
}

void GetFileTreeOperation::transitionToNextState()
{
    if(state() == BasicState::Ready) {
        m_currentPath = m_rootPath;
        setState(State::PreparingNextOperation);
        QTimer::singleShot(0, this, &GetFileTreeOperation::transitionToNextState);

    } else if(state() == State::PreparingNextOperation) {
        setState(State::RunningOperation);

        auto *op = device()->storage()->list(m_currentPath);
        connect(op, &AbstractOperation::finished, this, &GetFileTreeOperation::transitionToNextState);

    } else if(state() == State::RunningOperation) {
        auto *op = qobject_cast<ListOperation*>(sender());

        if(op->isError()) {
            finishWithError(op->errorString());
            return;
        }

        for(const auto &fileInfo : qAsConst(op->result())) {
            qDebug() << "Name:" << fileInfo.name << "Type:" << (int)fileInfo.type << "Size:" << fileInfo.size;
        }

        finishWithError("Just a drill");
    }
}
