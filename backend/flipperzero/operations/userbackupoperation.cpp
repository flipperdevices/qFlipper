#include "userbackupoperation.h"

#include "flipperzero/flipperzero.h"
#include "getfiletreeoperation.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

UserBackupOperation::UserBackupOperation(FlipperZero *device, const QString &path, QObject *parent):
    Operation(device, parent),
    m_backupPath(path)
{}

const QString UserBackupOperation::description() const
{
    return QStringLiteral("Backup user data @%1 %2").arg(device()->model(), device()->name());
}

void UserBackupOperation::transitionToNextState()
{

    auto *op = new GetFileTreeOperation(device(), "/ext", this);

    connect(op, &AbstractOperation::finished, this, [=]() {
        if(op->isError()) {
            finishWithError(op->errorString());
        } else {
            for(const auto &fileInfo: qAsConst(op->result())) {
                qDebug() << "Path:" << fileInfo.absolutePath << "Type:" << (int)fileInfo.type << "Size:" << fileInfo.size;
            }
        }

        op->deleteLater();
    });

    op->start();

    // Get device file list
    // 1. Issue storage list command
    // 2. Put every directory in queue
    // 3. Go to 1
    // 4. Put files and directories in resulting list

    // Download device files
    // Create backup directory
    // Store files locally
}

void UserBackupOperation::onOperationTimeout()
{
    qDebug() << "Operation timeout (UserBackupOperation)";
}
