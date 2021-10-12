#include "wirelessstackupdateoperation.h"

#include <QFile>
#include <QBuffer>
#include <QFileInfo>

#include "flipperzero/devicestate.h"
#include "flipperzero/radiomanifest.h"

#include "flipperzero/utilityinterface.h"
#include "flipperzero/utility/restartoperation.h"
#include "flipperzero/utility/startrecoveryoperation.h"

#include "flipperzero/recoveryinterface.h"
#include "flipperzero/recovery/setbootmodeoperation.h"
#include "flipperzero/recovery/exitrecoveryoperation.h"
#include "flipperzero/recovery/wirelessstackdownloadoperation.h"

#include "gzipuncompressor.h"
#include "tempdirectories.h"
#include "tararchive.h"
#include "macros.h"

using namespace Flipper;
using namespace Zero;

WirelessStackUpdateOperation::WirelessStackUpdateOperation(RecoveryInterface *recovery, UtilityInterface *utility, DeviceState *state, const QString &filePath, QObject *parent):
    AbstractTopLevelOperation(state, parent),
    m_recovery(recovery),
    m_utility(utility),
    m_compressedFile(new QFile(filePath, this)),
    m_uncompressedFile(new QFile(tempDirs()->root().absoluteFilePath(QFileInfo(*m_compressedFile).baseName() + QStringLiteral(".tar")), this)),
    m_radioFile(new QBuffer(this))
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
        setOperationState(WirelessStackUpdateOperation::ReadingFirmware);
        readFirmware();

    } else if(operationState() == WirelessStackUpdateOperation::ReadingFirmware) {
        setOperationState(WirelessStackUpdateOperation::StartingRecovery);
        startRecoveryMode();

    } else if(operationState() == WirelessStackUpdateOperation::StartingRecovery) {
        setOperationState(WirelessStackUpdateOperation::SettingRecoveryBootMode);
        setRecoveryBootMode();

    } else if(operationState() == WirelessStackUpdateOperation::SettingRecoveryBootMode) {
        setOperationState(WirelessStackUpdateOperation::UpdatingWirelessStack);
        updateWirelessStack();

    } else if(operationState() == WirelessStackUpdateOperation::UpdatingWirelessStack) {
        setOperationState(WirelessStackUpdateOperation::SettingOSBootMode);
        setOSBootMode();

    } else if(operationState() == WirelessStackUpdateOperation::SettingOSBootMode) {
        cleanup();
        finish();
    }
}

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

void WirelessStackUpdateOperation::readFirmware()
{
    TarArchive tar(m_uncompressedFile);
    const auto manifext = tar.fileData(QStringLiteral("core2_firmware/Manifest.json"));

    if(tar.isError()) {
        finishWithError(tar.errorString());
        return;
    }

    RadioManifest manifest(manifext);
    if(manifest.isError()) {
        finishWithError(manifest.errorString());
        return;
    }

    const auto fileName = QStringLiteral("core2_firmware/%1").arg(manifest.firmware().radio().files().first().name()); // Wowie!

    if(!m_radioFile->open(QIODevice::WriteOnly)) {
        finishWithError(m_radioFile->errorString());
    } else if(m_radioFile->write(tar.fileData(fileName)) <= 0) {
        finishWithError(QStringLiteral("Failed to extract firmware file: %1").arg(tar.errorString()));
    } else {
        advanceOperationState();
    }

    m_radioFile->close();
}

void WirelessStackUpdateOperation::startRecoveryMode()
{
    registerOperation(m_utility->startRecoveryMode());
}

void WirelessStackUpdateOperation::setRecoveryBootMode()
{
    registerOperation(m_recovery->setRecoveryBootMode());
}

void WirelessStackUpdateOperation::updateWirelessStack()
{
    registerOperation(m_recovery->downloadWirelessStack(m_radioFile));
}

void WirelessStackUpdateOperation::setOSBootMode()
{
    registerOperation(m_recovery->setOSBootMode());
}

void WirelessStackUpdateOperation::cleanup()
{
    m_uncompressedFile->remove();
}
