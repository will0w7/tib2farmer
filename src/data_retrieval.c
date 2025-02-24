#include <tchar.h>

#include "data_retrieval.h"
#include "utils.h"

#define OBF_MAGIC_NUMBER 443799

Currencies getCurrencies(const GameData* gameData) {
    Currencies currencies = {0};
    const uintptr_t gameBaseAddress = getModuleBaseAddress(gameData->moduleName, gameData->pID);
    if (gameBaseAddress == 0)
        return currencies;

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x100};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle))
        return currencies;

    uintptr_t currenciesAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(currenciesAddress + pointsOffsets[i], &currenciesAddress, sizeof(currenciesAddress), gameData->processHandle))
            return currencies;
    }
    currenciesAddress += pointsOffsets[7];

    if (!readMemory(currenciesAddress, &currencies, sizeof(currencies), gameData->processHandle))
        return currencies;

    currencies.credits += OBF_MAGIC_NUMBER;
    currencies.skulls += OBF_MAGIC_NUMBER;
    currencies.techPoints += OBF_MAGIC_NUMBER;
    return currencies;
}

Resources getResources(const GameData* gameData) {
    Resources resources = {0};
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(gameData->moduleName), gameData->pID);
    if (gameBaseAddress == 0) {
        return resources;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0xF0, 0x20, 0x80, 0x18};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return resources;
    }

    uintptr_t resourcesAddress = baseAddress;
    for (int i = 0; i < 8; i++) {
        if (!readMemory(resourcesAddress + pointsOffsets[i], &resourcesAddress, sizeof(resourcesAddress), gameData->processHandle)) {
            return resources;
        }
    }
    resourcesAddress += pointsOffsets[8];

    if (!readMemory(resourcesAddress, &resources, sizeof(resources), gameData->processHandle)) {
        return resources;
    }
    return resources;
}

Relics getRelics(const GameData* gameData) {
    Relics relics = {0};
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(gameData->moduleName), gameData->pID);
    if (gameBaseAddress == 0) {
        return relics;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0xF0, 0x20, 0x11C};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return relics;
    }

    uintptr_t relicsAddress = baseAddress;
    for (int i = 0; i < 7; i++) {
        if (!readMemory(relicsAddress + pointsOffsets[i], &relicsAddress, sizeof(relicsAddress), gameData->processHandle)) {
            return relics;
        }
    }
    relicsAddress += pointsOffsets[7];

    if (!readMemory(relicsAddress, &relics, sizeof(relics), gameData->processHandle)) {
        return relics;
    }
    return relics;
}

float getPlayerLevel(const GameData* gameData) {
    float playerLevel = 0.0f;
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(gameData->moduleName), gameData->pID);
    if (gameBaseAddress == 0) {
        return playerLevel;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01C95608;
    const uintptr_t pointsOffsets[] = {0xD0, 0x8, 0x80, 0x70, 0x78, 0x80, 0x28};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return playerLevel;
    }

    uintptr_t levelAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        if (!readMemory(levelAddress + pointsOffsets[i], &levelAddress, sizeof(levelAddress), gameData->processHandle)) {
            return playerLevel;
        }
    }
    levelAddress += pointsOffsets[6];

    if (!readMemory(levelAddress, &playerLevel, sizeof(playerLevel), gameData->processHandle)) {
        return playerLevel;
    }
    return playerLevel;
}

int getPlayerExp(const GameData* gameData) {
    int playerExp = 0;
    const uintptr_t gameBaseAddress = getModuleBaseAddress(_T(gameData->moduleName), gameData->pID);
    if (gameBaseAddress == 0) {
        return playerExp;
    }

    const uintptr_t offsetGameToBaseAddress = 0x01CA6330;
    const uintptr_t pointsOffsets[] = {0x160, 0x8, 0x70, 0x0, 0x28, 0x30, 0x108};

    uintptr_t baseAddress = 0;
    if (!readMemory(gameBaseAddress + offsetGameToBaseAddress, &baseAddress, sizeof(baseAddress), gameData->processHandle)) {
        return playerExp;
    }

    uintptr_t levelAddress = baseAddress;
    for (int i = 0; i < 6; i++) {
        if (!readMemory(levelAddress + pointsOffsets[i], &levelAddress, sizeof(levelAddress), gameData->processHandle)) {
            return playerExp;
        }
    }
    levelAddress += pointsOffsets[6];

    if (!readMemory(levelAddress, &playerExp, sizeof(playerExp), gameData->processHandle)) {
        return playerExp;
    }
    return playerExp + OBF_MAGIC_NUMBER;
}
