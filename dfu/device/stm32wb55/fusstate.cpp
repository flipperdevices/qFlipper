#include "fusstate.h"

namespace STM32 {
namespace WB55 {

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

}}
