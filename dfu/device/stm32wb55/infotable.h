#pragma once

#include <cstdint>

// AN5185 by STM32, pg. 19-20
// https://www.st.com/resource/en/application_note/dm00513965-st-firmware-upgrade-services-for-stm32wb-series-stmicroelectronics.pdf

namespace STM32 {
namespace WB55 {

#pragma pack(push)

namespace Subtables {
struct VersionInfo
{
    uint8_t build:4;
    uint8_t branch:4;
    uint8_t sub;
    uint8_t minor;
    uint8_t major;
};

struct MemoryInfo
{
    uint8_t flashSectorsUsed;
    uint8_t reserved;
    uint8_t SRAM2aSectorsUsed;
    uint8_t SRAM2bSectorsUsed;
};
}

struct FUSDeviceInfoTable
{
    uint32_t magic;
    uint8_t reserved1;

    uint8_t   lastFUSActiveState;
    uint8_t   lastWirelessStackState;
    uint8_t   wirelessStackType;

    Subtables::VersionInfo safeBootVersion;

    Subtables::VersionInfo FUSVersion;
    Subtables::MemoryInfo FUSMemorySize;

    Subtables::VersionInfo wirelessStackVersion;
    Subtables::MemoryInfo  wirelessStackMemorySize;

    uint32_t  wirelessFirmwareBleInfo;
    uint32_t  wirelessFirmwareThreadInfo;

    uint32_t  reserved2;
    uint64_t  UID64;
    uint16_t  deviceID;
    uint32_t  padding_ /* padding for 32 bits build */;
};

struct DeviceInfoTable {
    Subtables::VersionInfo safeBootVersion;

    struct {
        Subtables::VersionInfo version;
        Subtables::MemoryInfo memory;
        uint32_t info;
    } FUS;

    struct {
        Subtables::VersionInfo version;
        Subtables::MemoryInfo memory;
        uint32_t info;
        uint32_t reserved;
    } WirelessStack;
};

#pragma pack(pop)

static_assert (sizeof(Subtables::VersionInfo) == 4, "Check your struct packing!");
static_assert (sizeof(Subtables::MemoryInfo) == 4, "Check your struct packing!");
static_assert (sizeof(FUSDeviceInfoTable) == 56, "Check your struct packing!");
static_assert (sizeof(DeviceInfoTable) == 32, "Check your struct packing!");

}}
