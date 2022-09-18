#include "fusstate.h"

namespace STM32 {
namespace WB55 {

FUSState::FUSState(Status s, Error e):
    m_status(s),
    m_error(e)
{
    // Some statuses will have a range of values, normalising them here
    const auto regular = m_status == Idle || m_status == ErrorOccured || m_status == Invalid;
    m_status = regular? m_status : static_cast<Status>(m_status & 0xFFFFFFF0U);
}

FUSState::Status FUSState::status() const
{
    return m_status;
}

FUSState::Error FUSState::error() const
{
    return m_error;
}

bool FUSState::isValid() const {
    return m_status != Invalid;
}

const QString FUSState::statusString() const
{
    switch(m_status) {
        case Idle: return "Idle";
        case FWUpgradeOngoing: return "Firmware upgrade ongoing";
        case ServiceOngoing: return "Service Ongoing";
        case ErrorOccured: return "Error occurred";
        case Invalid: default: return "Invalid state";
    }
}

const QString FUSState::errorString() const
{
    switch (m_error) {
        case NoError: return "No error";
        case ImageNotFound: return "Image not found";
        case ImageCorrupt: return "Image corrupt";
        case ImageNotAuthentic: return "Image not authentic";
        case NotEnoughSpace: return "Not enough space";
        case UserAbort: return "User abort";
        case EraseError: return "Erase error";
        case WriteError: return "Write error";
        case STTagNotFound: return "ST Microelectronics tag not found";
        case CustomTagNotFound: return "User-specified tag not found";
        case AuthKeyLocked: return "Auth key locked";
        case RollBackError: return "Rollback error";
        case NotRunning: return "Not running";
        case Unknown: default: return "Unknown error";
    }
}

}}
