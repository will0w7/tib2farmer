#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <Windows.h>
#include <stdbool.h>

typedef struct {
    HWND hGameWindow;
    DWORD pID;
    HANDLE processHandle;
} GameData;

HWND getWindow();
DWORD getPID(HWND hGameWindow);
HANDLE getHandle(DWORD pID);
bool initGameData(GameData* gameData);

#endif
