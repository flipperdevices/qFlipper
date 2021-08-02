#pragma once

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
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress = 0);
    ~WirelessStackDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
    uint32_t m_targetAddress;
};

class FixBootIssuesOperation : public AbstractFirmwareOperation
{
public:
    FixBootIssuesOperation(FlipperZero *device);
    ~FixBootIssuesOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
};

class CheckOptionBytesOperation : public AbstractFirmwareOperation
{
public:
    CheckOptionBytesOperation(FlipperZero *device, QIODevice *file);
    ~CheckOptionBytesOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
};

}
}
