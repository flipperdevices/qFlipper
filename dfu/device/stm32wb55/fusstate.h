#pragma once

#include <QString>

namespace STM32 {
namespace WB55 {

class FUSState {
public:

    enum Status : uint32_t {
        Idle = 0x00,
        FWUpgradeOngoing = 0x10,
        FUSUpgradeOngoing = 0x20,
        ServiceOngoing = 0x30,
        ErrorOccured = 0xFF,
        Invalid = 0x0BADF00D
    };

    enum Error : uint32_t {
        NoError = 0x00,
        ImageNotFound,
        ImageCorrupt,
        ImageNotAuthentic,
        NotEnoughSpace,
        UserAbort,
        EraseError,
        WriteError,
        STTagNotFound,
        CustomTagNotFound,
        AuthKeyLocked,
        RollBackError = 0x11,
        NotRunning = 0xFE,
        Unknown = 0xFF
    };

    FUSState() = default;
    FUSState(Status s, Error e);

    Status status() const;
    Error error() const;
    bool isValid() const;

    const QString statusString() const;
    const QString errorString() const;

private:
    Status m_status = Invalid;
    Error m_error = Unknown;
};

}}
