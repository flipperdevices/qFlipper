#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {

class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public AbstractFirmwareOperation
{
    Q_OBJECT

    enum State {
        WaitForDFU = AbstractFirmwareOperation::User,
        Downloading,
        WaitForVCP,
    };

public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file, QObject *parent = nullptr);
    ~FirmwareDownloadOperation();

    const QString name() const override;
    void start() override;

private slots:
    void transitionToNextState();

private:
    FlipperZero *m_device;
    QIODevice *m_file;
};

//class WirelessStackDownloadOperation : public AbstractFirmwareOperation
//{
//public:
//    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress = 0);
//    ~WirelessStackDownloadOperation();

//    const QString name() const override;
//    bool execute() override;

//private:
//    FlipperZero *m_device;
//    QIODevice *m_file;
//    uint32_t m_targetAddress;
//};

//class FixBootIssuesOperation : public AbstractFirmwareOperation
//{
//public:
//    FixBootIssuesOperation(FlipperZero *device);
//    ~FixBootIssuesOperation();

//    const QString name() const override;
//    bool execute() override;

//private:
//    FlipperZero *m_device;
//};

//class FixOptionBytesOperation : public AbstractFirmwareOperation
//{
//public:
//    FixOptionBytesOperation(FlipperZero *device, QIODevice *file);
//    ~FixOptionBytesOperation();

//    const QString name() const override;
//    bool execute() override;

//private:
//    FlipperZero *m_device;
//    QIODevice *m_file;
//};

}
}
