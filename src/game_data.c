#include "game_data.h"
#include "utils.h"
#include <string.h>

#define GAME_NAME "TheInfiniteBlack2"
#define MODULE_NAME "UnityPlayer.dll"

HWND getWindow() {
    HWND hGameWindow = FindWindow(NULL, GAME_NAME);
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
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, pID);
    if (processHandle == NULL) {
        printError("OpenProcess");
        return NULL;
    }
    return processHandle;
}

bool initGameData(GameData* gameData) {
    int len = strlen(MODULE_NAME) + 1;
    gameData->moduleName = (char*)malloc(len);
    if (gameData->moduleName == NULL) {
        printError("Memory allocation failed for moduleName");
        return false;
    }
    strcpy_s(gameData->moduleName, len, MODULE_NAME);

    gameData->hGameWindow = getWindow();
    if (gameData->hGameWindow == NULL) {
        return false;
    }

    gameData->pID = getPID(gameData->hGameWindow);
    if (gameData->pID == 0) {
        return false;
    }

    gameData->processHandle = getHandle(gameData->pID);
    if (gameData->processHandle == NULL) {
        return false;
    }

    return true;
}
