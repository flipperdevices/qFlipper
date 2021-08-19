#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include <SetupAPI.h>
#include <Newdev.h>

#include <libwdi.h>

#define BIG_ENOUGH 1024

#define FLIPPERZERO_VID 0x0483
#define FLIPPERZERO_PID 0xdf11
#define FLIPPERZERO_DESC "Flipper Zero in DFU mode"

#define MANUFACTURER_DESC "Flipper Devices Inc."

#define DRIVER_DIR "usb_driver"

#define INCOMPATIBLE_INF "sttube.inf"
#define FLIPPERZERO_INF "flipzerodfu.inf"
#define DRIVER_STORE_PATH "C:\\Windows\\System32\\DriverStore\\FileRepository"

void print_last_error()
{
    LPSTR msg = NULL;

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL );

    printf("Error: %s", msg);
    LocalFree(msg);
}

int get_store_inf_folder(const char *inf_name, char *found_folder)
{
    int result_length = 0;

    char path[MAX_PATH];
    strcpy_s(path, MAX_PATH, DRIVER_STORE_PATH);
    strcat_s(path, MAX_PATH, "\\*");

    WIN32_FIND_DATAA data;
    HANDLE found = FindFirstFileA(path, &data);

    if(found == INVALID_HANDLE_VALUE) {
        print_last_error();
        return -1;
    }

    do {
        if(strstr(data.cFileName, inf_name)) {
            result_length = (int)strlen(data.cFileName);

            if(found_folder) {
                strcpy_s(found_folder, BIG_ENOUGH, data.cFileName);
            }

            break;
        }

    } while(FindNextFileA(found, &data));

    FindClose(found);
    return result_length;
}

int uninstall_misconfigured_devices(HWND hwnd)
{
    const GUID wanted_GUID = {0x36fc9e60, 0xc465, 0x11cf, {0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};
    HDEVINFO info_set = SetupDiGetClassDevsA(&wanted_GUID, NULL, NULL, 0);

    if(info_set == INVALID_HANDLE_VALUE) {
        print_last_error();
        return -1;
    }

    SP_DEVINFO_DATA info_data;
    info_data.cbSize = sizeof(SP_DEVINFO_DATA);

    int uninstall_count = 0;

    for(DWORD i = 0; SetupDiEnumDeviceInfo(info_set, i, &info_data); ++i) {
        BOOL result;
        DWORD size;
        char buf[1024];
        result = SetupDiGetDeviceInstanceIdA(info_set, &info_data, buf, sizeof(buf), &size);

        if(result == FALSE) {
            print_last_error();
            return -1;

        } else if(strstr(buf, "VID_0483&PID_DF11") == NULL) {
            continue;
        }

        printf("Uninstalling misconfigured device %s...\n", buf);

        result = DiUninstallDevice(hwnd, info_set, &info_data, 0, NULL);

        if(result == FALSE) {
            print_last_error();
            return -1;

        } else {
            printf("Device successfully uninstalled.\n");
            uninstall_count++;
        }
    }

    if(uninstall_count == 0) {
        printf("No misconfigured devices found.\n");
    }

    SetupDiDestroyDeviceInfoList(info_set);
    return 0;
}

int uninstall_incompatible_driver(HWND hwnd)
{
    int uninstall_count = 0;
    char inf_folder[BIG_ENOUGH];

    while(get_store_inf_folder(INCOMPATIBLE_INF, inf_folder) > 0) {
        char path[MAX_PATH];

        strcpy_s(path, MAX_PATH, DRIVER_STORE_PATH);
        strcat_s(path, MAX_PATH, "\\");
        strcat_s(path, MAX_PATH, inf_folder);
        strcat_s(path, MAX_PATH, "\\");
        strcat_s(path, MAX_PATH, INCOMPATIBLE_INF);

        printf("Uninstalling incompatible driver at \"%s\"...\n", inf_folder);

        if(DiUninstallDriverA(hwnd, path, 0, NULL) == FALSE) {
            print_last_error();
            return -1;

        } else {
            printf("Incompatible driver successfully uninstalled.\n");
            ++uninstall_count;
        }
    }

    if(uninstall_count == 0) {
        printf("No incompatible drivers found.\n");
    }

    return 0;
}

int install_flipper_driver(HWND hwnd)
{
    const int name_length = get_store_inf_folder(FLIPPERZERO_INF, NULL);
    if(name_length > 0) {
        printf("Flipper Zero DFU driver is already installed.\n");
        return 0;

    } else if(name_length < 0) {
        return -1;
    }

    printf("Installing Flipper Zero DFU driver...\n");

    struct wdi_device_info device_info = { NULL, FLIPPERZERO_VID, FLIPPERZERO_PID, FALSE, 0, FLIPPERZERO_DESC, NULL, NULL, NULL };
    struct wdi_options_prepare_driver prepare_options = { WDI_WINUSB, MANUFACTURER_DESC, NULL, FALSE, FALSE, NULL, FALSE };

    int err = wdi_prepare_driver(&device_info, DRIVER_DIR, FLIPPERZERO_INF, &prepare_options);

    if(err != WDI_SUCCESS) {
        printf("Error: %s\n", wdi_strerror(err));
        return err;
    }

    struct wdi_options_install_driver install_options = {hwnd, FALSE, 0 };
    err = wdi_install_driver(&device_info, DRIVER_DIR, FLIPPERZERO_INF, &install_options);

    if(err != WDI_SUCCESS) {
        printf("Error: %s\n", wdi_strerror(err));
        return err;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    HWND parent_handle = NULL;

    if(argc > 1) {
        parent_handle = (HWND)(uintptr_t)strtol(argv[1], NULL, 0);
    }

    if(uninstall_misconfigured_devices(parent_handle)) {
        printf("Error: failed to uninstall misconfigured device(s).\n");
        return -1;
    }

    if(uninstall_incompatible_driver(parent_handle)) {
        printf("Error: Failed to uninstall incompatible driver(s).\n");
        return -2;
    }

    if(install_flipper_driver(parent_handle)) {
        printf("Error: failed to install Flipper Zero driver.\n");
        return -3;
    }

    printf("All done. Have a nice day!\n");
    return 0;
}
