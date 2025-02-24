#include "utils.h"
#include <TlHelp32.h>
#include <tchar.h>
#include <Windows.h>
#include "pdc_config.h" // IWYU pragma: keep
#include "../include/curses.h"

void printError(const char* message) {
    DWORD errorCode = GetLastError();
    LPSTR errorMessage = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)&errorMessage, 0, NULL);
    fprintf(stderr, "%s: Error %lu: %s\n", message, errorCode, errorMessage);
    LocalFree(errorMessage);

    if (curscr != NULL) {
        wattron(curscr, COLOR_PAIR(3));
        wprintw(curscr, "%s: Error %lu: %s\n", message, errorCode, errorMessage);
        wattroff(curscr, COLOR_PAIR(3));
        refresh();
    }
}

uintptr_t getModuleBaseAddress(char* lpszModuleName, DWORD pID) {
    uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printError("CreateToolhelp32Snapshot");
        return 0;
    }

    MODULEENTRY32 ModuleEntry32 = {0};
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
                dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    } else {
        printError("Module32First");
    }

    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

bool readMemory(uintptr_t address, void* buffer, size_t size, HANDLE processHandle) {
    if (!ReadProcessMemory(processHandle, (LPCVOID)address, buffer, size, NULL)) {
        printError("ReadProcessMemory");
        return false;
    }
    return true;
}
