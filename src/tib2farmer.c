#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <math.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <minwindef.h>
#include <tchar.h>
#include <winuser.h>
#include <profileapi.h>
#include <errhandlingapi.h>
#include <corecrt.h>

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif
// #define PDC_BUILD_CONSOLE
#define PDC_WIDE 1
#define PDC_FORCE_UTF8 1
#include <curses.h>

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

HANDLE processHandle = NULL;

uintptr_t getModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, pID);
    MODULEENTRY32 ModuleEntry32 = {0};
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
                dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

HWND getWindow() {
    HWND hGameWindow = NULL;
    hGameWindow = FindWindow(NULL, "TheInfiniteBlack2");
    return hGameWindow;
}

DWORD getPID(HWND hGameWindow) {
    DWORD pID = 0;
    GetWindowThreadProcessId(hGameWindow, &pID);
    return pID;
}

bool getHandle(DWORD pID) {
    processHandle = OpenProcess(PROCESS_VM_READ, false, pID);
    return processHandle != INVALID_HANDLE_VALUE && processHandle != NULL;
}

Currencies getCurrencies(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x100};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t currenciesAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(currenciesAddress + pointsOffsets[i]), &currenciesAddress, sizeof(currenciesAddress), NULL);
    }
    currenciesAddress += pointsOffsets[7];

    Currencies currencies = {};
    ReadProcessMemory(processHandle, (LPVOID)currenciesAddress, &currencies, sizeof(currencies), NULL);
    currencies.credits += MAGIC_NUMBER_CURRENCIES;
    currencies.skulls += MAGIC_NUMBER_CURRENCIES;
    currencies.techPoints += MAGIC_NUMBER_CURRENCIES;
    return currencies;
}

Resources getResources(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x18};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t resourcesAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(resourcesAddress + pointsOffsets[i]), &resourcesAddress, sizeof(resourcesAddress), NULL);
    }
    resourcesAddress += pointsOffsets[8];

    Resources resources = {};
    ReadProcessMemory(processHandle, (LPVOID)resourcesAddress, &resources, sizeof(resources), NULL);
    return resources;
}

float getPlayerLevel(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0x80, 0x28};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t credsAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(credsAddress + pointsOffsets[i]), &credsAddress, sizeof(credsAddress), NULL);
    }
    credsAddress += pointsOffsets[6];

    float buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)credsAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getCredits(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x100};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t credsAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(credsAddress + pointsOffsets[i]), &credsAddress, sizeof(credsAddress), NULL);
    }
    credsAddress += pointsOffsets[7];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)credsAddress, &buffer, sizeof(buffer), NULL);
    return buffer + MAGIC_NUMBER_CURRENCIES;
}

int getSkulls(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x104};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t skullsAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(skullsAddress + pointsOffsets[i]), &skullsAddress, sizeof(skullsAddress), NULL);
    }
    skullsAddress += pointsOffsets[7];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)skullsAddress, &buffer, sizeof(buffer), NULL);
    return buffer + MAGIC_NUMBER_CURRENCIES;
}

int getTechPoints(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x108};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t tpAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(tpAddress + pointsOffsets[i]), &tpAddress, sizeof(tpAddress), NULL);
    }
    tpAddress += pointsOffsets[7];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)tpAddress, &buffer, sizeof(buffer), NULL);
    return buffer + MAGIC_NUMBER_CURRENCIES;
}

int getCrew(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x2C};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t crewAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(crewAddress + pointsOffsets[i]), &crewAddress, sizeof(crewAddress), NULL);
    }
    crewAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)crewAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getOrganics(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x18};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t organicsAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(organicsAddress + pointsOffsets[i]), &organicsAddress, sizeof(organicsAddress), NULL);
    }
    organicsAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)organicsAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getGas(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x1C};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t gasAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(gasAddress + pointsOffsets[i]), &gasAddress, sizeof(gasAddress), NULL);
    }
    gasAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)gasAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getMetals(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x20};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t metalsAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(metalsAddress + pointsOffsets[i]), &metalsAddress, sizeof(metalsAddress), NULL);
    }
    metalsAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)metalsAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getRadioactives(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x24};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t radioAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(radioAddress + pointsOffsets[i]), &radioAddress, sizeof(radioAddress), NULL);
    }
    radioAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)radioAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

int getDarkmatter(DWORD pID) {
    char moduleName[] = "UnityPlayer.dll";
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(moduleName), pID);

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x28};

    uintptr_t baseAddress = 0;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);

    uintptr_t darkmatterAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        ReadProcessMemory(processHandle, (LPVOID)(darkmatterAddress + pointsOffsets[i]), &darkmatterAddress, sizeof(darkmatterAddress), NULL);
    }
    darkmatterAddress += pointsOffsets[8];

    int buffer = 0;
    ReadProcessMemory(processHandle, (LPVOID)darkmatterAddress, &buffer, sizeof(buffer), NULL);
    return buffer;
}

void test() {
    HWND hGameWindow = getWindow();
    if (hGameWindow == NULL) {
        // escribir que debe abrir el juego y pulsar X tecla para volver a cargar (A de Attach)
        printf("hGameWindow null\n");
        return;
    }
    DWORD pID = getPID(hGameWindow);
    if (pID == 0) {
        // no se puede recuperar el pID del juego (?)
        printf("pID 0\n");
        return;
    }
    if (!getHandle(pID)) {
        // no se puede obtener un handle del proceso, ejecutandose como administrador?
        printf("HANDLE null\n");
        return;
    }
    int credits = getCredits(pID);
    printf("credits: %d\n", credits);
    int skulls = getSkulls(pID);
    printf("skulls: %d\n", skulls);
    int tp = getTechPoints(pID);
    printf("tp: %d\n", tp);
    int crew = getCrew(pID);
    printf("crew: %d\n", crew);
    int orgs = getOrganics(pID);
    printf("orgs: %d\n", orgs);
    int gas = getGas(pID);
    printf("gas: %d\n", gas);
    int metals = getMetals(pID);
    printf("metals: %d\n", metals);
    int radio = getRadioactives(pID);
    printf("radio: %d\n", radio);
    int dm = getDarkmatter(pID);
    printf("dm: %d\n", dm);

    printf("\n-----------------------------------\n\n");
    Currencies currencies = getCurrencies(pID);
    printf("credits: %d\n", currencies.credits);
    printf("skulls: %d\n", currencies.skulls);
    printf("tp: %d\n", currencies.techPoints);
    Resources resources = getResources(pID);
    printf("crew: %d\n", resources.crew);
    printf("organics: %d\n", resources.organics);
    printf("gas: %d\n", resources.gas);
    printf("metals: %d\n", resources.metals);
    printf("radioactives: %d\n", resources.radioactives);
    printf("darkmatter: %d\n", resources.darkmatter);
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

int getColorPairTot(int value) {
    if (value >= 0) {
        return 4;
    }
    return 8;
}

int main() {
    // test();
    // return 0;
    setlocale(LC_ALL, "");
    // _locale_t locale = _get_current_locale();
    // _locale_t locale = _create_locale(LC_NUMERIC, "C");
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

    HWND hGameWindow = getWindow();
    DWORD pID = getPID(hGameWindow);
    getHandle(pID);
    DWORD error = GetLastError();
    while (error != 0) {
        hGameWindow = getWindow();
        error = GetLastError();
        printf("[Initial check - Window] Error: %lu\n", error);
        if (error != 0) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("   UNABLE TO LOCATE WINDOW   ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            int input = getch();
            if (input == 'q') {
                exit(EXIT_FAILURE);
            }
            refresh();
            continue;
        }
        pID = getPID(hGameWindow);
        error = GetLastError();
        printf("[Initial check - PID] Error: %lu\n", error);
        if (error != 0) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("      UNABLE TO GET PID      ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            int input = getch();
            if (input == 'q') {
                exit(EXIT_FAILURE);
            }
            refresh();
            continue;
        }
        error = getHandle(pID);
        error = GetLastError();
        printf("[Initial check - Handle] Error: %lu\n", error);
        if (error != 0) {
            writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
            writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
            writeString("     UNABLE TO GET HANDLE    ", 14, 25, 5);
            writeString("Press 'Q' to exit", 23, 2, 0);
            int input = getch();
            if (input == 'q') {
                exit(EXIT_FAILURE);
            }
            refresh();
            continue;
        }
    }

    writeString("                             ", 12, 25, 0);
    writeString("                             ", 13, 25, 0);

    time_t startTime = time(NULL);
    time_t nowTime = time(NULL);
    int elapsedSeconds = 0;

    char timeBuffer[9];
    char intBuffer[20];
    char floatBuffer[32];

    const Currencies startCurrencies = getCurrencies(pID);
    const Resources startResources = getResources(pID);
    const float startPlayerLevel = getPlayerLevel(pID);

    Currencies currencies = {};
    Resources resources = {};
    float playerLevel = 0.0f;
    while (1) {
        erase();
        error = GetLastError();
        while (error != 0) {
            hGameWindow = getWindow();
            error = GetLastError();
            printf("[Main loop - Window] Error: %lu\n", error);
            if (error != 0) {
                writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
                writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
                writeString("   UNABLE TO LOCATE WINDOW   ", 14, 25, 5);
                writeString("Press 'Q' to exit", 23, 2, 0);
                int input = getch();
                if (input == 'q') {
                    exit(EXIT_FAILURE);
                }
                refresh();
                continue;
            }
            pID = getPID(hGameWindow);
            error = GetLastError();
            printf("[Main loop - PID] Error: %lu\n", error);
            if (error != 0) {
                writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
                writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
                writeString("      UNABLE TO GET PID      ", 14, 25, 5);
                writeString("Press 'Q' to exit", 23, 2, 0);
                int input = getch();
                if (input == 'q') {
                    exit(EXIT_FAILURE);
                }
                refresh();
                continue;
            }
            error = getHandle(pID);
            error = GetLastError();
            printf("[Main loop - Handle] Error: %lu\n", error);
            if (error != 0) {
                writeString("   CANNOT READ GAME STATE    ", 12, 25, 5);
                writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
                writeString("     UNABLE TO GET HANDLE    ", 14, 25, 5);
                writeString("Press 'Q' to exit", 23, 2, 0);
                int input = getch();
                if (input == 'q') {
                    exit(EXIT_FAILURE);
                }
                refresh();
                continue;
            }
        }
        writeString("                             ", 12, 25, 0);
        writeString("                             ", 13, 25, 0);

        currencies = getCurrencies(pID);
        resources = getResources(pID);

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

        // STATICS
        writeString("        Initial values       ", 1, 1, 4);

        // Currencies
        writeString("Credits:", 3, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startCurrencies.credits);
        writeString(intBuffer, 3, 16, getColorPair(startCurrencies.credits));

        writeString("Skulls:", 4, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startCurrencies.skulls);
        writeString(intBuffer, 4, 16, getColorPair(startCurrencies.skulls));

        writeString("Tech points:", 5, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startCurrencies.techPoints);
        writeString(intBuffer, 5, 16, getColorPair(startCurrencies.techPoints));

        // Resources
        writeString("Crew:", 7, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.crew);
        writeString(intBuffer, 7, 16, getColorPair(startResources.crew));

        writeString("Organics:", 8, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.organics);
        writeString(intBuffer, 8, 16, getColorPair(startResources.organics));

        writeString("Gas:", 9, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.gas);
        writeString(intBuffer, 9, 16, getColorPair(startResources.gas));

        writeString("Metals:", 10, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.metals);
        writeString(intBuffer, 10, 16, getColorPair(startResources.metals));

        writeString("Radioactives:", 11, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.radioactives);
        writeString(intBuffer, 11, 16, getColorPair(startResources.radioactives));

        writeString("Darkmatter:", 12, 2, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'12d", startResources.darkmatter);
        writeString(intBuffer, 12, 16, getColorPair(startResources.darkmatter));

        // DYNAMICS
        writeString("       Earnings      ", 1, 31, 4);

        // Currencies
        snprintf(intBuffer, sizeof(intBuffer), "%'11d", creds);
        writeString(intBuffer, 3, 40, getColorPairIm(creds));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", skulls);
        writeString(intBuffer, 4, 40, getColorPairIm(skulls));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", tps);
        writeString(intBuffer, 5, 40, getColorPairIm(tps));

        // Resources
        snprintf(intBuffer, sizeof(intBuffer), "%'11d", crew);
        writeString(intBuffer, 7, 40, getColorPairIm(crew));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", orgs);
        writeString(intBuffer, 8, 40, getColorPairIm(orgs));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", gas);
        writeString(intBuffer, 9, 40, getColorPairIm(gas));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", metals);
        writeString(intBuffer, 10, 40, getColorPairIm(metals));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", radio);
        writeString(intBuffer, 11, 40, getColorPairIm(radio));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", dm);
        writeString(intBuffer, 12, 40, getColorPairIm(dm));

        // Credits
        writeString("       Credits      ", 1, 53, 4);

        // Currencies
        snprintf(intBuffer, sizeof(intBuffer), "%'11d", creds * CREDITS_VALUE);
        writeString(intBuffer, 3, 61, getColorPair(creds));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", skulls * SKULLS_VALUE);
        writeString(intBuffer, 4, 61, getColorPair(skulls));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", tps * TP_VALUE);
        writeString(intBuffer, 5, 61, getColorPair(tps));

        // Resources
        snprintf(intBuffer, sizeof(intBuffer), "%'11d", crew * CREW_VALUE);
        writeString(intBuffer, 7, 61, getColorPair(crew));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", orgs * ORGANIC_VALUE);
        writeString(intBuffer, 8, 61, getColorPair(orgs));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", gas * GAS_VALUE);
        writeString(intBuffer, 9, 61, getColorPair(gas));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", metals * METAL_VALUE);
        writeString(intBuffer, 10, 61, getColorPair(metals));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", radio * RADIOACTIVE_VALUE);
        writeString(intBuffer, 11, 61, getColorPair(radio));

        snprintf(intBuffer, sizeof(intBuffer), "%'11d", dm * DARKMATTER_VALUE);
        writeString(intBuffer, 12, 61, getColorPair(dm));

        // Total
        const int total = creds * CREDITS_VALUE + skulls * SKULLS_VALUE + tps * TP_VALUE + crew * CREW_VALUE + orgs * ORGANIC_VALUE + gas * GAS_VALUE + metals * METAL_VALUE +
            radio * RADIOACTIVE_VALUE + dm * DARKMATTER_VALUE;
        writeString("Total:", 15, 50, 0);
        snprintf(intBuffer, sizeof(intBuffer), "%'11d", total);
        writeString(intBuffer, 15, 61, getColorPair(total));

        // Time
        writeString("Elapsed:", 23, 60, 0);
        writeString(timeBuffer, 23, 70, 1);

        // XP
        playerLevel = getPlayerLevel(pID);
        const float levelDelta = playerLevel - startPlayerLevel;
        const float expRate = levelDelta / elapsedSeconds;
        const float missingExp = (float)ceil(playerLevel) - playerLevel;
        int secondsToLevelUp = 0;
        // if (fabsf(expRate) > 1e-6) {
        if (expRate > 0) {
            secondsToLevelUp = (int)missingExp / expRate;
        }

        writeString("Player Level:", 17, 2, 0);
        snprintf(floatBuffer, sizeof(floatBuffer), "%'.7f", playerLevel);
        writeString(floatBuffer, 17, 18, 0);

        writeString("Leveled:", 18, 2, 0);
        snprintf(floatBuffer, sizeof(floatBuffer), "%'.7f", levelDelta);
        writeString(floatBuffer, 18, 18, 0);

        writeString("Time to level:", 19, 2, 0);
        if (secondsToLevelUp == 0) {
            snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", 99, 59, 59);
        } else {
            snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", secondsToLevelUp / 3600, (secondsToLevelUp % 3600) / 60, secondsToLevelUp % 60);
        }
        writeString(timeBuffer, 19, 18, 0);

        // END
        writeString("Press 'Q' to exit", 23, 2, 0);
        // Draw
        refresh();

        int input = getch();
        if (input == 'q') {
            break;
        }
    }
    endwin();

    if (processHandle != NULL) {
        CloseHandle(processHandle);
    }
    return EXIT_SUCCESS;
}
