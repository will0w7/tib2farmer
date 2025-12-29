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

void getCurrencies(const GameData* gameData, Currencies* currencies);
void getResources(const GameData* gameData, Resources* resources);
void getRelics(const GameData* gameData, Relics* relics);
float getPlayerLevel(const GameData* gameData, float playerLevel);
int getPlayerExp(const GameData* gameData, int playerExp);

#endif
