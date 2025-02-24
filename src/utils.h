#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <stdbool.h>

void printError(const char* message);
uintptr_t getModuleBaseAddress(char* lpszModuleName, DWORD pID);
bool readMemory(uintptr_t address, void* buffer, size_t size, HANDLE processHandle);

#endif
