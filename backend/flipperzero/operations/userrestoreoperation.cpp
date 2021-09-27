#include "userrestoreoperation.h"

#include <QUrl>
#include <QFile>
#include <QTimer>

#include "flipperzero/flipperzero.h"
#include "flipperzero/storagecontroller.h"
#include "flipperzero/storage/writeoperation.h"
#include "flipperzero/storage/removeoperation.h"

#include "macros.h"

using namespace Flipper;
using namespace Zero;

UserRestoreOperation::UserRestoreOperation(FlipperZero *device, const QString &backupPath, QObject *parent):
    Operation(device, parent),
    m_backupDir(QUrl(backupPath).toLocalFile()),
    m_deviceDirName(QByteArrayLiteral("/int"))
{}

const QString UserRestoreOperation::description() const
{
    return QStringLiteral("Restore user data @%1 %2").arg(device()->model(), device()->name());
}

void UserRestoreOperation::transitionToNextState()
{
    qDebug() << " = = = = = = = = Hello there!";
    finish();
}
