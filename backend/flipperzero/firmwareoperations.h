#ifndef FIRMWAREOPERATIONS_H
#define FIRMWAREOPERATIONS_H

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {
class FlipperZero;

namespace Zero {

class FirmwareOperation : public AbstractFirmwareOperation
{
public:
    FirmwareOperation(FlipperZero *device);
    virtual ~FirmwareOperation() {};

protected:
    void waitForReconnect(int timeoutMS = 10000);
    FlipperZero *m_device;
};

class FirmwareDownloadOperation : public FirmwareOperation
{
public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file);
    ~FirmwareDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    QIODevice *m_file;
};

class WirelessStackDownloadOperation : public FirmwareOperation
{
public:
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t addr);
    ~WirelessStackDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    QIODevice *m_file;
    uint32_t m_targetAddress;
};

}
}

#endif // FIRMWAREOPERATIONS_H
