#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

#include "client.dll.hpp"
#include "offsets.hpp"
#include "buttons.hpp"

static DWORD get_process_id(const wchar_t* process_name) {
    DWORD process_id = 0;

    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snap_shot == INVALID_HANDLE_VALUE) {
        return process_id;
    }

    PROCESSENTRY32 process_entry = {};
    process_entry.dwSize = sizeof(process_entry);

    if (Process32First(snap_shot, &process_entry)) {
        do {
            if (_wcsicmp(process_name, process_entry.szExeFile) == 0) {
                process_id = process_entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snap_shot, &process_entry));
    }

    CloseHandle(snap_shot);

    return process_id;
}

static std::uintptr_t get_module_base(const DWORD pid, const wchar_t* module_name) {
    std::uintptr_t module_base = 0;

    HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap_shot == INVALID_HANDLE_VALUE) {
        return module_base;
    }

    MODULEENTRY32W module_entry = {};
    module_entry.dwSize = sizeof(decltype(module_entry));

    if (Module32FirstW(snap_shot, &module_entry) == TRUE) {
        if (wcsstr(module_name, module_entry.szModule) != nullptr)
            module_base = reinterpret_cast<std::uintptr_t>(module_entry.modBaseAddr);
    }
    else {
        while (Module32NextW(snap_shot, &module_entry) == TRUE) {
            if (wcsstr(module_name, module_entry.szModule) != nullptr) {
                module_base = reinterpret_cast<std::uintptr_t>(module_entry.modBaseAddr);
                break;
            }
        }
    }

    CloseHandle(snap_shot);

    return module_base;
}

namespace driver {
    namespace codes {
        // Used to setup the driver
        constexpr ULONG attach =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG read =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

        constexpr ULONG write =
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    } // namespace codes

    // Shared between usermode and kernelmode
    struct Request {
        HANDLE process_id;

        PVOID target;
        PVOID buffer;

        SIZE_T size;
        SIZE_T return_size;
    };

    bool attach_to_process(HANDLE driver_handle, HANDLE process_handle) {
        Request r;
        r.process_id = process_handle;

        return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }

    template <class T>
    T read_memory(HANDLE driver_handle, const std::uintptr_t addr) {
        T temp = {};

        Request r;
        r.target = reinterpret_cast<PVOID>(addr);
        r.buffer = &temp;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::read, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);

        return temp;
    }

    template <class T>
    void write_memory(HANDLE driver_handle, const std::uintptr_t addr, const T& value) {
        Request r;
        r.target = reinterpret_cast<PVOID>(addr);
        r.buffer = (PVOID)&value;
        r.size = sizeof(T);

        DeviceIoControl(driver_handle, codes::write, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }
}

int main() {
    const DWORD pid = get_process_id(L"cs2.exe");

    if (pid == 0) {
        std::cout << "[-] Failed to find cs2.\n";
        std::cin.get();
        return 1;
    }

    const HANDLE driver = CreateFile(L"\\\\.\\AmangLyDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (driver == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Failed to get driver handle\n";
        std::cin.get();
        return 1;
    }

    if (driver::attach_to_process(driver, OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))) {
        std::cout << "[+] Attached Successfully\n";

        if (const std::uintptr_t client = get_module_base(pid, L"client.dll"); client != 0) {
            std::cout << "[+] Found client.dll\n";

            while (true) {
                if (GetAsyncKeyState(VK_END)) {
                    break;

                    const auto local_player_pawn = driver::read_memory<std::uintptr_t>(
                        driver, client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);

                    if (local_player_pawn == 0) {
                        continue;
                    }

                    const auto flags = driver::read_memory<std::uint32_t>(
                        driver, local_player_pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);

                    const bool in_air = flags & (1 << 0);
                    const bool space_pressed = GetAsyncKeyState(VK_SPACE);
                    const auto force_jump = driver::read_memory<std::uint32_t>(
                        driver, local_player_pawn + cs2_dumper::buttons::jump);

                    if (space_pressed && in_air) {
                        Sleep(5);
                        driver::write_memory(driver, client + cs2_dumper::buttons::jump, 65537);
                    }
                    else if (space_pressed && !in_air) {
                        driver::write_memory(driver, client + cs2_dumper::buttons::jump, 256);
                    }
                    else if (!space_pressed && force_jump == 65537) {
                        driver::write_memory(driver, client + cs2_dumper::buttons::jump, 256);
                    }
                }
            }
        }
    }

    CloseHandle(driver);

    std::cin.get();

    return 0;
}
