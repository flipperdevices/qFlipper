#include "usbdevicedetector_p.h"

#include <windows.h>
#include <setupapi.h>

#include "macros.h"

//static LRESULT CALLBACK hWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

USBDeviceDetectorWorker::USBDeviceDetectorWorker(const QList<USBDeviceInfo> &wanted, QObject *parent):
    QObject(parent),
    m_wanted(wanted)
{
    startTimer(1000);
}

void USBDeviceDetectorWorker::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);

    const auto available = availableDevices();
    processDevicesArrived(available);
    processDevicesLeft(available);
}

QList <USBDeviceInfo> USBDeviceDetectorWorker::availableDevices() const
{
    QList <USBDeviceInfo> ret;

    HDEVINFO infoSet = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE);
    check_return_val(infoSet != INVALID_HANDLE_VALUE, "Failed to get device list", ret);

    SP_DEVINFO_DATA infoData;
    infoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for(DWORD i = 0; SetupDiEnumDeviceInfo(infoSet, i, &infoData); ++i) {

        char buf[1024]; DWORD size;
        const auto result = SetupDiGetDeviceInstanceIdA(infoSet, &infoData, buf, sizeof(buf), &size);

        if(result == FALSE) {
            continue;
        }

        for(const auto &wantedInfo : m_wanted) {
            const size_t WANTED_BUF_SIZE = 22;
            char wantedBuf[WANTED_BUF_SIZE];

            snprintf(wantedBuf, WANTED_BUF_SIZE, "USB\\VID_%04X&PID_%04X", wantedInfo.vendorID(), wantedInfo.productID());

            if(strncmp(buf, wantedBuf, WANTED_BUF_SIZE - 1)) continue;

            SP_DEVICE_INTERFACE_DATA interfaceData;
            interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

            const GUID USBDeviceGUID = {0xa5dcbf10, 0x6530, 0x11d2, {0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed}};
            auto result = SetupDiEnumDeviceInterfaces(infoSet, &infoData, &USBDeviceGUID, 0, &interfaceData);

            if(result == FALSE) break;

            ULONG detailSize = 0;
            SetupDiGetDeviceInterfaceDetailA(infoSet, &interfaceData, nullptr, 0, &detailSize, nullptr);

            if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) break;

            auto *detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)LocalAlloc(LMEM_FIXED, detailSize);
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

            result = SetupDiGetDeviceInterfaceDetailA(infoSet, &interfaceData, detailData, detailSize, nullptr, nullptr);

            if(result == TRUE) {
                const auto newInfo = parseInstanceID(buf);
                ret.append(newInfo.withBackendData(QByteArray(detailData->DevicePath)));
            }

            LocalFree(detailData);
        }
    }

    if(infoSet) {
        SetupDiDestroyDeviceInfoList(infoSet);
    }

    return ret;
}

void USBDeviceDetectorWorker::processDevicesArrived(const QList<USBDeviceInfo> &available)
{
    for(const auto &info : available) {
        const auto it = std::find_if(m_current.cbegin(), m_current.cend(), [&](const USBDeviceInfo &arg) {
            return arg.backendData() == info.backendData();
        });

        if(it == m_current.cend()) {
            m_current.append(info);
            emit devicePluggedIn(info);
        }
    }
}

void USBDeviceDetectorWorker::processDevicesLeft(const QList<USBDeviceInfo> &available)
{
    for(auto currentIt = m_current.begin(); currentIt != m_current.end();) {
        const auto availableIt = std::find_if(available.cbegin(), available.cend(), [&](const USBDeviceInfo &arg) {
            return arg.backendData() == currentIt->backendData();
        });

        if(availableIt == available.cend()) {
            const auto info = *currentIt;
            emit deviceUnplugged(info);
            currentIt = m_current.erase(currentIt);

        } else {
            ++currentIt;
        }
    }
}

USBDeviceInfo USBDeviceDetectorWorker::parseInstanceID(const char *buf)
{
    unsigned int vid, pid;
    char serialbuf[1024];

    sscanf_s(buf, "USB\\VID_%04X&PID_%04X\\%s", &vid, &pid, serialbuf, (unsigned int)strlen(buf));
    return USBDeviceInfo(vid, pid).withSerialNumber(serialbuf);
}
