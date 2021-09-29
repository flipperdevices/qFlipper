#pragma once

#include "flipperzero/flipperzerooperation.h"

class QTimer;
class QIODevice;

namespace Flipper {
namespace Zero {

class WirelessStackDownloadOperation : public FlipperZeroOperation
{
    Q_OBJECT

    enum State {
        BootingToDFU = AbstractOperation::User,
        SettingDFUBoot,
        StartingFUS,
        DeletingWirelessStack,
        DownloadingWirelessStack,
        UpgradingWirelessStack,
        ResettingDFUBoot
    };

public:
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress = 0, QObject *parent = nullptr);
    ~WirelessStackDownloadOperation();

    const QString description() const override;

private slots:
    void transitionToNextState() override;
    void onOperationTimeout() override;

private:
    void bootToDFU();
    void setDFUBoot(bool set);
    void startFUS();
    void deleteWirelessStack();
    bool isWirelessStackDeleted();
    void downloadWirelessStack();
    void upgradeWirelessStack();
    bool isWirelessStackUpgraded();

    QIODevice *m_file;
    QTimer *m_loopTimer;
    uint32_t m_targetAddress;
};

}
}
