#pragma once

#include "abstractrecoveryoperation.h"

class QFile;
class QTimer;
class QBuffer;

namespace Flipper {
namespace Zero {

class RadioUpdateOperation : public AbstractRecoveryOperation
{
    Q_OBJECT

    enum OperationState {
        UnpackingArchive = AbstractOperation::User,
        StartingFUS,
        DeletingWirelessStack,
        DownloadingWirelessStack,
        UpgradingWirelessStack
    };

public:
    RadioUpdateOperation(Recovery *recovery, QFile *file, QObject *parent = nullptr);
    const QString description() const override;

private slots:
    void advanceOperationState() override;
    void onOperationTimeout() override;

private:
    void unpackArchive();
    void startFUS();
    void deleteWirelessStack();
    bool isWirelessStackDeleted();
    void downloadWirelessStack();
    void upgradeWirelessStack();
    bool isWirelessStackUpgraded();

    QFile *m_sourceFile;
    QBuffer *m_firmwareFile;
    QTimer *m_loopTimer;
};

}
}

