#pragma once

#include "abstractoperationhelper.h"
#include "flipperzero/radiomanifest.h"

class QFile;
class TarZipArchive;

namespace Flipper {
namespace Zero {

class RadioManifestHelper : public AbstractOperationHelper
{
    Q_OBJECT

    enum State {
        UncompressingArchive = AbstractOperationHelper::User,
        ReadingManifest
    };

public:
    RadioManifestHelper(QFile *radioArchive, QObject *parent = nullptr);

    int stackType() const;
    const QString &radioVersion() const;
    const QString &fusVersion() const;

    const QByteArray radioFirmwareData() const;
    const QByteArray fusFirmwareData(const QString &deviceFusVersion) const;

private:
    void nextStateLogic() override;

    void uncompressArchive();
    void readManifest();

    QFile *m_compressedFile;
    TarZipArchive *m_archive;
    RadioManifest m_manifest;
};

}
}

