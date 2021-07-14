#pragma once

namespace STM32 {
namespace WB55 {

class FUSState {
public:

    enum Status {
        Idle = 0x00,
        FWUpgradeOngoing = 0x10,
        FUSUpgradeOngoing = 0x20,
        ServiceOngoing = 0x30,
        ErrorOccured = 0xFF,
        Invalid = 0x0BADF00D
    };

    enum Error {
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
    FUSState(Status s, Error e):
        m_status(s),
        m_error(e)
    {}

    Status status() const;
    Error error() const;
    bool isValid() const;

private:
    Status m_status = Invalid;
    Error m_error = Unknown;
};

}}
