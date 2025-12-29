#include <tchar.h>

#include "data_retrieval.h"
#include "utils.h"

#define OBF_MAGIC_NUMBER 443799

#define UNITY_MODULE_NAME "UnityPlayer.dll"
#define MONO_MODULE_NAME "mono-2.0-bdwgc.dll"

void getCurrencies(const GameData* gameData, Currencies* currencies) {
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(MONO_MODULE_NAME), gameData->pID);
    if (gameBaseAddress == 0)
        return;

    const uintptr_t offsetGameToBaseAddress = 0x0073A0F8;
    const uintptr_t pointsOffsets[] = {0x190, 0xDD8, 0x18, 0x20, 0x38, 0x100, 0x20, 0x100};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle))
        return;

    uintptr_t currenciesAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(currenciesAddress + pointsOffsets[i], &currenciesAddress, sizeof(currenciesAddress), gameData->processHandle))
            return;
    }
    currenciesAddress += pointsOffsets[7];

    if (!readMemory(currenciesAddress, currencies, sizeof(*currencies), gameData->processHandle))
        return;

    currencies->credits += OBF_MAGIC_NUMBER;
    currencies->skulls += OBF_MAGIC_NUMBER;
    currencies->techPoints += OBF_MAGIC_NUMBER;
    currencies->starShards += OBF_MAGIC_NUMBER;
}

void getResources(const GameData* gameData, Resources* resources) {
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(MONO_MODULE_NAME), gameData->pID);
    if (gameBaseAddress == 0) {
        return;
    }

    const uintptr_t offsetGameToBaseAddress = 0x0073A0F8;
    const uintptr_t pointsOffsets[] = {0x190, 0xDD8, 0x18, 0x20, 0x38, 0x100, 0x20, 0x80, 0x18};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return;
    }

    uintptr_t resourcesAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        if (!readMemory(resourcesAddress + pointsOffsets[i], &resourcesAddress, sizeof(resourcesAddress), gameData->processHandle)) {
            return;
        }
    }
    resourcesAddress += pointsOffsets[8];

    if (!readMemory(resourcesAddress, resources, sizeof(*resources), gameData->processHandle)) {
        return;
    }
}

void getRelics(const GameData* gameData, Relics* relics) {
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(MONO_MODULE_NAME), gameData->pID);
    if (gameBaseAddress == 0) {
        return;
    }

    const uintptr_t offsetGameToBaseAddress = 0x0073A0F8;
    const uintptr_t pointsOffsets[] = {0x190, 0xDD8, 0x18, 0x20, 0x38, 0x100, 0x20, 0x120};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return;
    }

    uintptr_t relicsAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(relicsAddress + pointsOffsets[i], &relicsAddress, sizeof(relicsAddress), gameData->processHandle)) {
            return;
        }
    }
    relicsAddress += pointsOffsets[7];

    if (!readMemory(relicsAddress, relics, sizeof(*relics), gameData->processHandle)) {
        return;
    }
    return;
}

void getPlayerLevel(const GameData* gameData, float* playerLevel) {
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(MONO_MODULE_NAME), gameData->pID);
    if (gameBaseAddress == 0) {
        return;
    }

    const uintptr_t offsetGameToBaseAddress = 0x0073A0F8;
    const uintptr_t pointsOffsets[] = {0x190, 0xDD8, 0x18, 0x20, 0x38, 0x80, 0x28};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return;
    }

    uintptr_t levelAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        if (!readMemory(levelAddress + pointsOffsets[i], &levelAddress, sizeof(levelAddress), gameData->processHandle)) {
            return;
        }
    }
    levelAddress += pointsOffsets[6];

    if (!readMemory(levelAddress, playerLevel, sizeof(*playerLevel), gameData->processHandle)) {
        return;
    }
    return;
}

void getPlayerExp(const GameData* gameData, int* playerExp) {
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(MONO_MODULE_NAME), gameData->pID);
    if (gameBaseAddress == 0) {
        return;
    }

    const uintptr_t offsetGameToBaseAddress = 0x0073A0F8;
    const uintptr_t pointsOffsets[] = {0x190, 0xDD8, 0x18, 0x20, 0x38, 0x30, 0x140};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return;
    }

    uintptr_t levelAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        if (!readMemory(levelAddress + pointsOffsets[i], &levelAddress, sizeof(levelAddress), gameData->processHandle)) {
            return;
        }
    }
    levelAddress += pointsOffsets[6];

    if (!readMemory(levelAddress, playerExp, sizeof(*playerExp), gameData->processHandle)) {
        return;
    }
    *playerExp += OBF_MAGIC_NUMBER;
    return;
}
