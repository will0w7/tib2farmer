#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <winuser.h>
#include <errhandlingapi.h>

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif
#define PDC_WIDE 1
#define PDC_FORCE_UTF8 1
#include "../include/curses.h"

#define TARGET_FPS 2
#define TICK_DURATION 1000 / TARGET_FPS

#define MAGIC_NUMBER_CURRENCIES 443799

#define CREDITS_VALUE 1
#define SKULLS_VALUE 700
#define TP_VALUE 1000

typedef struct {
    int credits;
    int skulls;
    int techPoints;
} Currencies;

#define CREW_VALUE 100 / 100
#define ORGANIC_VALUE 400 / 100
#define GAS_VALUE 800 / 100
#define METAL_VALUE 1200 / 100
#define RADIOACTIVE_VALUE 1600 / 100
#define DARKMATTER_VALUE 2000 / 100

typedef struct {
    int organics;
    int gas;
    int metals;
    int radioactives;
    int darkmatter;
    int crew;
} Resources; // This struct follows the game memory layout

#define HUMAN_VALUE 5000
#define WYRD_VALUE 1500
#define HETS_VALUE 1500
#define PREC_VALUE 5000

typedef struct {
    int humans;
    int wyrds;
    int hets;
    int precursors;
} Relics;

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

uintptr_t getModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
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

HWND getWindow() {
    HWND hGameWindow = FindWindow(NULL, "TheInfiniteBlack2");
    if (hGameWindow == NULL) {
        printError("FindWindow");
    }
    return hGameWindow;
}

DWORD getPID(HWND hGameWindow) {
    DWORD pID = 0;
    GetWindowThreadProcessId(hGameWindow, &pID);
    if (pID == 0) {
        printError("GetWindowThreadProcessId");
    }
    return pID;
}

HANDLE getHandle(DWORD pID) {
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pID);
    if (processHandle == NULL) {
        printError("OpenProcess");
        return NULL;
    }
    return processHandle;
}

bool readMemory(uintptr_t address, void* buffer, size_t size, HANDLE processHandle) {
    if (!ReadProcessMemory(processHandle, (LPCVOID)address, buffer, size, NULL)) {
        printError("ReadProcessMemory");
        return false;
    }
    return true;
}

Currencies getCurrencies(DWORD pID, HANDLE processHandle) {
    Currencies currencies = {0};
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);
    if (gameBaseAddress == 0)
        return currencies;

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x100};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), processHandle))
        return currencies;

    uintptr_t currenciesAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(currenciesAddress + pointsOffsets[i], &currenciesAddress, sizeof(currenciesAddress), processHandle))
            return currencies;
    }
    currenciesAddress += pointsOffsets[7];

    if (!readMemory(currenciesAddress, &currencies, sizeof(currencies), processHandle))
        return currencies;

    currencies.credits += MAGIC_NUMBER_CURRENCIES;
    currencies.skulls += MAGIC_NUMBER_CURRENCIES;
    currencies.techPoints += MAGIC_NUMBER_CURRENCIES;
    return currencies;
}

Resources getResources(DWORD pID, HANDLE processHandle) {
    Resources resources = {0};
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);
    if (gameBaseAddress == 0) {
        return resources;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x18};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), processHandle)) {
        return resources;
    }

    uintptr_t resourcesAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        if (!readMemory(resourcesAddress + pointsOffsets[i], &resourcesAddress, sizeof(resourcesAddress), processHandle)) {
            return resources;
        }
    }
    resourcesAddress += pointsOffsets[8];

    if (!readMemory(resourcesAddress, &resources, sizeof(resources), processHandle)) {
        return resources;
    }
    return resources;
}

Relics getRelics(DWORD pID, HANDLE processHandle) {
    Relics relics = {0};
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);
    if (gameBaseAddress == 0) {
        return relics;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x11C};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), processHandle)) {
        return relics;
    }

    uintptr_t relicsAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(relicsAddress + pointsOffsets[i], &relicsAddress, sizeof(relicsAddress), processHandle)) {
            return relics;
        }
    }
    relicsAddress += pointsOffsets[7];

    if (!readMemory(relicsAddress, &relics, sizeof(relics), processHandle)) {
        return relics;
    }
    return relics;
}

float getPlayerLevel(DWORD pID, HANDLE processHandle) {
    float playerLevel = 0.0f;
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);
    if (gameBaseAddress == 0) {
        return playerLevel;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0x80, 0x28};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), processHandle)) {
        return playerLevel;
    }

    uintptr_t levelAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        if (!readMemory(levelAddress + pointsOffsets[i], &levelAddress, sizeof(levelAddress), processHandle)) {
            return playerLevel;
        }
    }
    levelAddress += pointsOffsets[6];

    if (!readMemory(levelAddress, &playerLevel, sizeof(playerLevel), processHandle)) {
        return playerLevel;
    }
    return playerLevel;
}

void writeString(const char* string, int y, int x, int pair) {
    attron(COLOR_PAIR(pair));
    mvprintw(y, x, string);
    attroff(COLOR_PAIR(pair));
}

int getColorPair(int value) {
    if (value > 0) {
        return 2;
    } else if (value == 0) {
        return 1;
    }
    return 3;
}

int getColorPairIm(int value) {
    if (value > 0) {
        return 6;
    } else if (value == 0) {
        return 1;
    }
    return 7;
}

char* formatInt(int num) {
    static char formattedNum[32];
    snprintf(formattedNum, sizeof(formattedNum), "%d", num); // Convertir el número a string sin espacios
    size_t len = strlen(formattedNum);

    size_t formattedLen = len + (len - 1) / 3;

    if (formattedLen + 1 > sizeof(formattedNum)) {
        return formattedNum;
    }

    char result[32];
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if ((len - i) % 3 == 0 && i != 0) {
            result[j++] = ',';
        }
        result[j++] = formattedNum[i];
    }
    result[j] = '\0';

    int spaces = 12 - formattedLen;
    if (spaces < 0)
        spaces = 0;

    snprintf(formattedNum, sizeof(formattedNum), "%*s", 12, result);

    return formattedNum;
}

char* formatFloat(float num, int precision) {
    static char formattedNum[64];
    char integerPart[32];
    char decimalPart[32];

    float intPart;
    float decPart = modff(num, &intPart);

    snprintf(integerPart, sizeof(integerPart), "%.0f", intPart);
    size_t lenInt = strlen(integerPart);

    snprintf(decimalPart, sizeof(decimalPart), "%.*f", precision, decPart);
    size_t lenDec = strlen(decimalPart) - 2; // Longitud sin "0."

    size_t formattedLen = lenInt + (lenInt - 1) / 3 + 1 + lenDec;

    if (formattedLen + 1 > sizeof(formattedNum)) {
        snprintf(formattedNum, sizeof(formattedNum), "%.*f", precision, num);
        return formattedNum;
    }

    char result[64];
    int j = 0;

    for (size_t i = 0; i < lenInt; i++) {
        if ((lenInt - i) % 3 == 0 && i != 0) {
            result[j++] = ',';
        }
        result[j++] = integerPart[i];
    }

    result[j++] = '.';

    for (size_t i = 0; i < lenDec; i++) {
        result[j++] = decimalPart[i + 2];
    }
    result[j] = '\0';

    int spaces = 12 - formattedLen;
    if (spaces < 0)
        spaces = 0;
    snprintf(formattedNum, sizeof(formattedNum), "%*s", 12, result);

    return formattedNum;
}

int main() {
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    cbreak();
    start_color();
    curs_set(0);
    timeout(TICK_DURATION);
    raw();

    // Color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_WHITE, COLOR_RED);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_RED, COLOR_WHITE);

    HWND hGameWindow = NULL;
    DWORD pID = 0;
    HANDLE processHandle = NULL;

    bool running = true;
    while (running) {
        if ((hGameWindow = getWindow()) == NULL) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("   UNABLE TO LOCATE WINDOW   ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            refresh();
            int input = getch();
            if (input == 'q') {
                running = false;
            }
            napms(500);
            continue;
        }

        if ((pID = getPID(hGameWindow)) == 0) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("      UNABLE TO GET PID      ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            refresh();
            int input = getch();
            if (input == 'q') {
                running = true;
            }
            napms(500);
            continue;
        }

        if ((processHandle = getHandle(pID)) == NULL) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("     UNABLE TO GET HANDLE    ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            refresh();
            int input = getch();
            if (input == 'q') {
                running = true;
            }
            napms(500);
            continue;
        }

        writeString("                            ", 12, 25, 0);
        writeString("                            ", 13, 25, 0);
        writeString("                            ", 14, 25, 0);

        time_t startTime = time(NULL);
        time_t nowTime = time(NULL);
        int elapsedSeconds = 0;

        char timeBuffer[9];

        Currencies startCurrencies = getCurrencies(pID, processHandle);
        Resources startResources = getResources(pID, processHandle);
        Relics startRelics = getRelics(pID, processHandle);
        float startPlayerLevel = getPlayerLevel(pID, processHandle);

        Currencies currencies = {0};
        Resources resources = {0};
        Relics relics = {0};
        float playerLevel = 0.0f;

        while (running) {
            erase();

            currencies = getCurrencies(pID, processHandle);
            resources = getResources(pID, processHandle);
            relics = getRelics(pID, processHandle);
            playerLevel = getPlayerLevel(pID, processHandle);

            nowTime = time(NULL);
            elapsedSeconds = (int)floor(difftime(nowTime, startTime));
            snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", elapsedSeconds / 3600, (elapsedSeconds % 3600) / 60, elapsedSeconds % 60);

            const int creds = currencies.credits - startCurrencies.credits;
            const int skulls = currencies.skulls - startCurrencies.skulls;
            const int tps = currencies.techPoints - startCurrencies.techPoints;

            const int crew = resources.crew - startResources.crew;
            const int orgs = resources.organics - startResources.organics;
            const int gas = resources.gas - startResources.gas;
            const int metals = resources.metals - startResources.metals;
            const int radio = resources.radioactives - startResources.radioactives;
            const int dm = resources.darkmatter - startResources.darkmatter;

            const int humans = relics.humans - startRelics.humans;
            const int wyrds = relics.wyrds - startRelics.wyrds;
            const int hets = relics.hets - startRelics.hets;
            const int precursors = relics.precursors - startRelics.precursors;

            const int total = creds * CREDITS_VALUE + skulls * SKULLS_VALUE + tps * TP_VALUE + crew * CREW_VALUE + orgs * ORGANIC_VALUE + gas * GAS_VALUE + metals * METAL_VALUE +
                radio * RADIOACTIVE_VALUE + dm * DARKMATTER_VALUE;

            const float levelDelta = playerLevel - startPlayerLevel;
            const float expRate = levelDelta / elapsedSeconds;
            const float missingExp = (float)ceil(playerLevel) - playerLevel;
            int secondsToLevelUp = 0;
            if (expRate > 0) {
                secondsToLevelUp = (int)(missingExp / expRate);
            }
            int credsPerHour = 0;
            if (total > 0 && elapsedSeconds > 0) {
                credsPerHour = (int)floor(((float)total / elapsedSeconds) * 3600);
            }

            // STATICS
            writeString("       Initial values       ", 1, 1, 4);

            // Currencies
            writeString("Credits:", 3, 2, 0);
            writeString(formatInt(startCurrencies.credits), 3, 16, 0);
            writeString("Skulls:", 4, 2, 0);
            writeString(formatInt(startCurrencies.skulls), 4, 16, 0);
            writeString("Tech points:", 5, 2, 0);
            writeString(formatInt(startCurrencies.techPoints), 5, 16, 0);

            // Resources
            writeString("Crew:", 7, 2, 0);
            writeString(formatInt(startResources.crew), 7, 16, 0);
            writeString("Organics:", 8, 2, 0);
            writeString(formatInt(startResources.organics), 8, 16, 0);
            writeString("Gas:", 9, 2, 0);
            writeString(formatInt(startResources.gas), 9, 16, 0);
            writeString("Metals:", 10, 2, 0);
            writeString(formatInt(startResources.metals), 10, 16, 0);
            writeString("Radioactives:", 11, 2, 0);
            writeString(formatInt(startResources.radioactives), 11, 16, 0);
            writeString("Darkmatter:", 12, 2, 0);
            writeString(formatInt(startResources.darkmatter), 12, 16, 0);

            // Relics
            writeString("HumanR:", 14, 2, 0);
            writeString(formatInt(startRelics.humans), 14, 16, 0);
            writeString("WyrdR:", 15, 2, 0);
            writeString(formatInt(startRelics.wyrds), 15, 16, 0);
            writeString("HetR:", 16, 2, 0);
            writeString(formatInt(startRelics.hets), 16, 16, 0);
            writeString("PrecR:", 17, 2, 0);
            writeString(formatInt(startRelics.precursors), 17, 16, 0);

            // DYNAMICS
            writeString("      Earnings       ", 1, 31, 4);

            // Currencies
            writeString(formatInt(creds), 3, 39, getColorPairIm(creds));
            writeString(formatInt(skulls), 4, 39, getColorPairIm(skulls));
            writeString(formatInt(tps), 5, 39, getColorPairIm(tps));

            // Resources
            writeString(formatInt(crew), 7, 39, getColorPairIm(crew));
            writeString(formatInt(orgs), 8, 39, getColorPairIm(orgs));
            writeString(formatInt(gas), 9, 39, getColorPairIm(gas));
            writeString(formatInt(metals), 10, 39, getColorPairIm(metals));
            writeString(formatInt(radio), 11, 39, getColorPairIm(radio));
            writeString(formatInt(dm), 12, 39, getColorPairIm(dm));

            // Relics
            writeString(formatInt(humans), 14, 39, getColorPair(humans));
            writeString(formatInt(wyrds), 15, 39, getColorPair(wyrds));
            writeString(formatInt(hets), 16, 39, getColorPair(hets));
            writeString(formatInt(precursors), 17, 39, getColorPair(precursors));

            // CREDITS
            writeString("      Credits      ", 1, 54, 4);

            // Currencies
            writeString(formatInt(creds * CREDITS_VALUE), 3, 60, getColorPair(creds));
            writeString(formatInt(skulls * SKULLS_VALUE), 4, 60, getColorPair(skulls));
            writeString(formatInt(tps * TP_VALUE), 5, 60, getColorPair(tps));

            // Resources
            writeString(formatInt(crew * CREW_VALUE), 7, 60, getColorPair(crew));
            writeString(formatInt(orgs * ORGANIC_VALUE), 8, 60, getColorPair(orgs));
            writeString(formatInt(gas * GAS_VALUE), 9, 60, getColorPair(gas));
            writeString(formatInt(metals * METAL_VALUE), 10, 60, getColorPair(metals));
            writeString(formatInt(radio * RADIOACTIVE_VALUE), 11, 60, getColorPair(radio));
            writeString(formatInt(dm * DARKMATTER_VALUE), 12, 60, getColorPair(dm));

            // Relics
            writeString(formatInt(humans * HUMAN_VALUE), 14, 60, getColorPair(humans));
            writeString(formatInt(wyrds * WYRD_VALUE), 15, 60, getColorPair(wyrds));
            writeString(formatInt(hets * HETS_VALUE), 16, 60, getColorPair(hets));
            writeString(formatInt(precursors * PREC_VALUE), 17, 60, getColorPair(precursors));

            // Total
            writeString("Total:", 19, 50, 0);
            writeString(formatInt(total), 19, 60, getColorPair(total));

            // Creds per hour
            writeString("Creds/h:", 20, 50, 0);
            writeString(formatInt(credsPerHour), 20, 60, getColorPair(total));

            // Time
            writeString("Elapsed:", 23, 53, 0);
            writeString(timeBuffer, 23, 64, 1);

            // XP
            writeString("Player Level:", 19, 2, 0);
            writeString(formatFloat(playerLevel, 7), 19, 18, 0);

            writeString("Leveled:", 20, 2, 0);
            writeString(formatFloat(levelDelta, 7), 20, 18, 0);

            writeString("Time to level:", 21, 2, 0);
            if (secondsToLevelUp == 0) {
                snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", 99, 59, 59);
            } else {
                snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", secondsToLevelUp / 3600, (secondsToLevelUp % 3600) / 60, secondsToLevelUp % 60);
            }
            writeString(timeBuffer, 21, 22, 6);

            // END
            writeString("Press 'Q' to exit", 23, 2, 3);
            // Draw
            refresh();

            int input = getch();
            if (input == 'q') {
                running = false;
            }

            if (!IsWindow(hGameWindow)) {
                printError("Lost Game Window");
                break;
            }
        }
    }

    endwin();

    if (processHandle != NULL) {
        CloseHandle(processHandle);
    }
    return EXIT_SUCCESS;
}
