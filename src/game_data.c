#include "game_data.h"
#include "utils.h"
#include <string.h>

#define GAME_NAME "TheInfiniteBlack2"
#define UNITY_MODULE_NAME "UnityPlayer.dll"
#define MONO_MODULE_NAME "mono-2.0-bdwgc.dll"

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
    int len = strlen(MONO_MODULE_NAME) + 1;
    gameData->monoModuleName = (char*)malloc(len);
    if (gameData->monoModuleName == NULL) {
        printError("Memory allocation failed for MONO_MODULE_NAME");
        return false;
    }
    strcpy_s(gameData->monoModuleName, len, MONO_MODULE_NAME);

    len = strlen(UNITY_MODULE_NAME) + 1;
    gameData->unityModuleName = (char*)malloc(len);
    if (gameData->unityModuleName == NULL) {
        printError("Memory allocation failed for UNITY_MODULE_NAME");
        return false;
    }
    strcpy_s(gameData->unityModuleName, len, UNITY_MODULE_NAME);

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
