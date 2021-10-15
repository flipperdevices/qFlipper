#pragma once

#include "abstractoperationhelper.h"

#include <QByteArray>

class QFile;
class TarZipArchive;

namespace Flipper {
namespace Zero {

class ScriptsHelper : public AbstractOperationHelper
{
    Q_OBJECT

    enum State {
        UncompressingArchive = AbstractOperationHelper::User
    };

public:
    ScriptsHelper(QFile *scriptsArchive, QObject *parent = nullptr);

    const QByteArray optionBytesData() const;

private:
    void nextStateLogic() override;
    void uncompressArchive();

    QFile *m_compressedFile;
    TarZipArchive *m_archive;
};

}
}

