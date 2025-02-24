
#include <string.h>
#include <math.h>

#include "ui.h"
#include "data_retrieval.h"
#include <Windows.h>
#include "pdc_config.h" // IWYU pragma: keep
#include "../include/curses.h"

#define CREDITS_VALUE 1
#define SKULLS_VALUE 750
#define TP_VALUE 1000

#define CREW_VALUE 100 / 100
#define ORGANIC_VALUE 400 / 100
#define GAS_VALUE 800 / 100
#define METAL_VALUE 1200 / 100
#define RADIOACTIVE_VALUE 1600 / 100
#define DARKMATTER_VALUE 2000 / 100

#define HUMAN_VALUE 5000
#define WYRD_VALUE 1500
#define HETS_VALUE 1500
#define PREC_VALUE 5000

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
    snprintf(formattedNum, sizeof(formattedNum), "%d", num);
    size_t len = strlen(formattedNum);

    int isNegative = 0;

    if (num < 0) {
        isNegative = 1;
        len--;
    }

    size_t formattedLen = len + (len - 1) / 3;

    if (formattedLen + isNegative + 1 > sizeof(formattedNum)) {
        return formattedNum;
    }

    char result[32];
    size_t j = 0;
    size_t k = 0;

    if (isNegative) {
        result[j++] = '-';
        k++;
    }

    for (size_t i = 0; i < len; i++) {
        if ((len - i) % 3 == 0 && i != 0) {
            result[j++] = ',';
        }
        result[j++] = formattedNum[k++];
    }
    result[j] = '\0';

    int spaces = 12 - strlen(result);
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

    if (num < 0) {
        lenInt--;
    }

    snprintf(decimalPart, sizeof(decimalPart), "%.*f", precision, decPart);
    size_t lenDec = strlen(decimalPart) - 2;

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

void drawUI(const GameData* gameData, const Timer* timer, const Currencies* startCurrencies, const Resources* startResources, const Relics* startRelics, float startPlayerLevel,
            int startPlayerExp) {
    char timeBuffer[9];
    Currencies currencies = getCurrencies(gameData);
    Resources resources = getResources(gameData);
    Relics relics = getRelics(gameData);
    float playerLevel = getPlayerLevel(gameData);
    int playerExp = getPlayerExp(gameData);

    const int creds = currencies.credits - startCurrencies->credits;
    const int skulls = currencies.skulls - startCurrencies->skulls;
    const int tps = currencies.techPoints - startCurrencies->techPoints;

    const int crew = resources.crew - startResources->crew;
    const int orgs = resources.organics - startResources->organics;
    const int gas = resources.gas - startResources->gas;
    const int metals = resources.metals - startResources->metals;
    const int radio = resources.radioactives - startResources->radioactives;
    const int dm = resources.darkmatter - startResources->darkmatter;

    const int humans = relics.humans - startRelics->humans;
    const int wyrds = relics.wyrds - startRelics->wyrds;
    const int hets = relics.hets - startRelics->hets;
    const int precursors = relics.precursors - startRelics->precursors;

    const int total = creds * CREDITS_VALUE + skulls * SKULLS_VALUE + tps * TP_VALUE + crew * CREW_VALUE + orgs * ORGANIC_VALUE + gas * GAS_VALUE + metals * METAL_VALUE +
        radio * RADIOACTIVE_VALUE + dm * DARKMATTER_VALUE;

    const float levelDelta = playerLevel - startPlayerLevel;
    const float expRate = levelDelta / timer->elapsedSeconds;
    const float missingExp = (float)ceil(playerLevel) - playerLevel;
    int secondsToLevelUp = 0;
    if (expRate > 0) {
        secondsToLevelUp = (int)(missingExp / expRate);
    }
    int credsPerHour = 0;
    if (total > 0 && timer->elapsedSeconds > 0) {
        credsPerHour = (int)floor(((float)total / timer->elapsedSeconds) * 3600);
    }
    int totalPlayerExp = playerExp - startPlayerExp;
    int expPerHour = 0;
    if (totalPlayerExp > 0 && timer->elapsedSeconds > 0) {
        expPerHour = (int)floor(((float)totalPlayerExp / timer->elapsedSeconds) * 3600);
    }

    // STATICS
    writeString("        Initial values       ", 1, 1, 4);

    // Currencies
    writeString("Credits:", 3, 2, 0);
    writeString(formatInt(startCurrencies->credits), 3, 16, 0);
    writeString("Skulls:", 4, 2, 0);
    writeString(formatInt(startCurrencies->skulls), 4, 16, 0);
    writeString("Tech points:", 5, 2, 0);
    writeString(formatInt(startCurrencies->techPoints), 5, 16, 0);

    // Resources
    writeString("Crew:", 7, 2, 0);
    writeString(formatInt(startResources->crew), 7, 16, 0);
    writeString("Organics:", 8, 2, 0);
    writeString(formatInt(startResources->organics), 8, 16, 0);
    writeString("Gas:", 9, 2, 0);
    writeString(formatInt(startResources->gas), 9, 16, 0);
    writeString("Metals:", 10, 2, 0);
    writeString(formatInt(startResources->metals), 10, 16, 0);
    writeString("Radioactives:", 11, 2, 0);
    writeString(formatInt(startResources->radioactives), 11, 16, 0);
    writeString("Darkmatter:", 12, 2, 0);
    writeString(formatInt(startResources->darkmatter), 12, 16, 0);

    // Relics
    writeString("HumanR:", 14, 2, 0);
    writeString(formatInt(startRelics->humans), 14, 16, 0);
    writeString("WyrdR:", 15, 2, 0);
    writeString(formatInt(startRelics->wyrds), 15, 16, 0);
    writeString("HetR:", 16, 2, 0);
    writeString(formatInt(startRelics->hets), 16, 16, 0);
    writeString("PrecR:", 17, 2, 0);
    writeString(formatInt(startRelics->precursors), 17, 16, 0);

    // DYNAMICS
    writeString("       Earnings       ", 1, 31, 4);

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
    writeString("       Credits       ", 1, 54, 4);

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

    // XP
    writeString("Player Level:", 19, 2, 0);
    writeString(formatFloat(playerLevel, 7), 19, 18, 0);

    writeString("Leveled:", 20, 2, 0);
    writeString(formatFloat(levelDelta, 7), 20, 18, 0);

    writeString("Time to level:", 21, 2, 0);
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", secondsToLevelUp / 3600, (secondsToLevelUp % 3600) / 60, secondsToLevelUp % 60);
    writeString(timeBuffer, 21, 22, 6);

    writeString("Exp/h:", 22, 2, 0);
    writeString(formatInt(expPerHour), 22, 18, 6);

    // END
    writeString("Elapsed:", 24, 53, 0);
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", timer->elapsedSeconds / 3600, (timer->elapsedSeconds % 3600) / 60, timer->elapsedSeconds % 60);
    writeString(timeBuffer, 24, 64, 1);

    writeString("Press P to pause/resume the timer", 24, 2, 0);
    writeString("Press 'Q' to exit", 25, 2, 3);

    refresh();
}
