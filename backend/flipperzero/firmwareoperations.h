#ifndef FIRMWAREOPERATIONS_H
#define FIRMWAREOPERATIONS_H

#include "firmwareoperation.h"

class QIODevice;

namespace Flipper {
class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public FirmwareOperation
{
public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file);
    ~FirmwareDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    void waitForReconnect();

    FlipperZero *m_device;
    QIODevice *m_file;
};

}
}

#endif // FIRMWAREOPERATIONS_H
