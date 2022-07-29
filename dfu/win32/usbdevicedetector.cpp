#include "usbdevicedetector.h"

#include <windows.h>
#include <setupapi.h>
#include <dbt.h>

#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LOG_DETECTOR, "USB")

static LRESULT CALLBACK hotplugWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct USBDeviceDetector::USBDeviceDetectorPrivate
{
    HINSTANCE appInstsance;
    ATOM hotplugWindowAtom;
    HWND hotplugWindowHandle;
};

USBDeviceDetector::USBDeviceDetector(QObject *parent):
    QObject(parent),
    m_p(new USBDeviceDetectorPrivate)
{
    if(!registerAtom()) {
        qCCritical(LOG_DETECTOR) <<  "Failed to register hotplug window atom";
    } else if(!createHotplugWindow()) {
        qCCritical(LOG_DETECTOR) <<  "Failed to create hotplug window";
    }
}

USBDeviceDetector::~USBDeviceDetector()
{
    delete m_p;
}

void USBDeviceDetector::setLogLevel(int logLevel) {
    Q_UNUSED(logLevel)
    qCInfo(LOG_DETECTOR) << "Setting USB backend log level is not implemented on WinUSB";
}

bool USBDeviceDetector::setWantedDevices(const QList<USBDeviceInfo> &wantedList)
{
    m_wanted = wantedList;
    update();
    return true;
}

void USBDeviceDetector::update()
{
    const auto available = availableDevices();
    processDevicesArrived(available);
    processDevicesLeft(available);
}

bool USBDeviceDetector::registerAtom()
{
    m_p->appInstsance = GetModuleHandleA(nullptr);

    if(!m_p->appInstsance) {
        qCCritical(LOG_DETECTOR) <<  "Failed to get application instance";
        return false;
    }

    WNDCLASSEXA wc;
    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = hotplugWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_p->appInstsance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "qFlipper";
    wc.hIconSm = nullptr;

    m_p->hotplugWindowAtom = RegisterClassExA(&wc);

    return m_p->hotplugWindowAtom != INVALID_ATOM;
}

bool USBDeviceDetector::createHotplugWindow()
{
    m_p->hotplugWindowHandle = CreateWindowExA(WS_EX_CLIENTEDGE, "qFlipper", "qFlipper", WS_EX_OVERLAPPEDWINDOW,
                                      100, 100, 200, 200, nullptr, nullptr, m_p->appInstsance, nullptr);

    const auto success = (m_p->hotplugWindowHandle != INVALID_HANDLE_VALUE);

    if(success) {
        SetWindowLongPtrA(m_p->hotplugWindowHandle, GWLP_USERDATA, (LONG_PTR)this);
        ShowWindow(m_p->hotplugWindowHandle, SW_HIDE);
        EnableWindow(m_p->hotplugWindowHandle, TRUE);
    }

    return success;
}

QList<USBDeviceInfo> USBDeviceDetector::availableDevices() const
{
    QList <USBDeviceInfo> ret;

    HDEVINFO infoSet = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE);

    if(infoSet == INVALID_HANDLE_VALUE) {
        qCCritical(LOG_DETECTOR) <<  "Failed to get device list";
        return ret;
    }

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

void USBDeviceDetector::processDevicesArrived(const QList<USBDeviceInfo> &available)
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

void USBDeviceDetector::processDevicesLeft(const QList<USBDeviceInfo> &available)
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

USBDeviceInfo USBDeviceDetector::parseInstanceID(const char *buf)
{
    unsigned int vid, pid;
    char serialbuf[1024];

    sscanf_s(buf, "USB\\VID_%04X&PID_%04X\\%s", &vid, &pid, serialbuf, (unsigned int)strlen(buf));
    return USBDeviceInfo(vid, pid).withSerialNumber(serialbuf);
}

static LRESULT CALLBACK hotplugWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if((msg == WM_DEVICECHANGE) && (wParam == DBT_DEVNODES_CHANGED)) {
        if(auto *ctx = (USBDeviceDetector*)GetWindowLongPtrA(hWnd, GWLP_USERDATA)) {
            ctx->update();
        }

        return TRUE;

    } else if(msg == WM_CLOSE) {
        DestroyWindow(hWnd);
    } else if(msg == WM_DESTROY) {
        // Something...?
    } else {
        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    return FALSE;
}
