#pragma once

#include "abstractrecoveryoperation.h"

class QIODevice;

namespace Flipper {
namespace Zero {

class FirmwareDownloadOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum State {
        DownloadingFirmware = AbstractOperation::User
    };

public:
    FirmwareDownloadOperation(Recovery *recovery, QIODevice *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void nextStateLogic() override;

private:
    void downloadFirmware();

    QIODevice *m_file;
};

}
}

