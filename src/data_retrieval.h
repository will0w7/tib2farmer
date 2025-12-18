#ifndef DATA_RETRIEVAL_H
#define DATA_RETRIEVAL_H

#include "game_data.h"

typedef struct {
    int credits;
    int skulls;
    int techPoints;
    int starShards;
} Currencies;

typedef struct {
    int organics;
    int gas;
    int metals;
    int radioactives;
    int darkmatter;
    int crew;
} Resources;

typedef struct {
    int humans;
    int wyrds;
    int hets;
    int precursors;
} Relics;

Currencies getCurrencies(const GameData* gameData);
Resources getResources(const GameData* gameData);
Relics getRelics(const GameData* gameData);
float getPlayerLevel(const GameData* gameData);
int getPlayerExp(const GameData* gameData);

#endif
