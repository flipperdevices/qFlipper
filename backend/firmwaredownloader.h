#pragma once

#include <QObject>

#include "flipperupdates.h"

class QIODevice;

namespace Flipper {

class FlipperZero;
class FirmwareDownloader : public QObject
{
    Q_OBJECT

public:
    FirmwareDownloader(QObject *parent = nullptr);

public slots:
    void downloadLocalFile(Flipper::FlipperZero *device, const QString &filePath);
    void downloadRemoteFile(Flipper::FlipperZero *device, const Flipper::Updates::VersionInfo &versionInfo);

    void downloadLocalFUS(Flipper::FlipperZero *device, const QString &filePath);
    void downloadLocalWirelessStack(Flipper::FlipperZero *device, const QString &filePath);

    void fixBootIssues(Flipper::FlipperZero *device);
    void fixOptionBytes(Flipper::FlipperZero *device, const QString &filePath);

    void downloadAssets(Flipper::FlipperZero *device, const QString &filePath);

    void backupUserData(Flipper::FlipperZero *device, const QString &backupPath);
    void restoreUserData(Flipper::FlipperZero *device, const QString &backupPath);
};

}
