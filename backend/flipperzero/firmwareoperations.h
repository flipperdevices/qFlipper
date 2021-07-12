#ifndef FIRMWAREOPERATIONS_H
#define FIRMWAREOPERATIONS_H

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {
class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public AbstractFirmwareOperation
{
public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file);
    ~FirmwareDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
};

class WirelessStackDownloadOperation : public AbstractFirmwareOperation
{
public:
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t addr = 0);
    ~WirelessStackDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
    uint32_t m_targetAddress;
};

}
}

#endif // FIRMWAREOPERATIONS_H
