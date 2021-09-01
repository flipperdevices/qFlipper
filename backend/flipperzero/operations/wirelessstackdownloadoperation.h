#pragma once

#include "abstractfirmwareoperation.h"

class QTimer;
class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class WirelessStackDownloadOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        BootingToDFU = AbstractFirmwareOperation::User,
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

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();
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
    void finish();

    FlipperZero *m_device;
    QIODevice *m_file;
    QTimer *m_loopTimer;
    uint32_t m_targetAddress;
};

}
}
