#include "wirelessstackupdateoperation.h"

#include <QFile>
#include <QFileInfo>

#include "flipperzero/devicestate.h"
#include "flipperzero/radiomanifest.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"

#include "gzipuncompressor.h"
#include "tempdirectories.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

WirelessStackUpdateOperation::WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_compressedFile(new QFile(filePath, this)),
    m_uncompressedFile(new QFile(TempDirectories::instance()->tempRoot().absoluteFilePath(QFileInfo(*m_compressedFile).baseName() + QStringLiteral(".tar")), this))
{}

const QString WirelessStackUpdateOperation::description() const
{
    return QStringLiteral("Wireless Stack Update @%1").arg(deviceState()->name());
}

void WirelessStackUpdateOperation::nextStateLogic()
{
    if(operationState() == AbstractOperation::Ready) {
        setOperationState(WirelessStackUpdateOperation::ExtractingBundle);
        extractBundle();

    } else if(operationState() == WirelessStackUpdateOperation::ExtractingBundle) {
        finish();
    }
}

void WirelessStackUpdateOperation::onSubOperationErrorOccured()
{}

void WirelessStackUpdateOperation::extractBundle()
{
    auto *uncompressor = new GZipUncompressor(m_compressedFile, m_uncompressedFile, this);

    if(uncompressor->isError()) {
        finishWithError(uncompressor->errorString());
        return;
    }

    connect(uncompressor, &GZipUncompressor::finished, this, [=]() {
        if(uncompressor->isError()) {
            finishWithError(uncompressor->errorString());
        } else {
            advanceOperationState();
        }
    });
}
