#ifndef UI_H
#define UI_H

#define TARGET_FPS 4
#define TICK_DURATION 1000 / TARGET_FPS

#include "game_data.h"
#include "timer.h"
#include "data_retrieval.h"

void drawUI(const Timer* timer, const Currencies* startCurrencies, const Resources* startResources, const Relics* startRelics, const float startPlayerLevel,
            const int startPlayerExp, const Currencies* currencies, const Resources* resources, const Relics* relics, const float playerLevel, const int playerExp);
void writeString(const char* string, int y, int x, int pair);
int getColorPair(int value);
int getColorPairIm(int value);
char* formatInt(int num);
char* formatFloat(float num, int precision);
void eraseErrors();
void printErrors(GameData* gameData);

#endif
