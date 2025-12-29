#include <locale.h>
#include "game_data.h"
#include "data_retrieval.h"
#include "timer.h"
#include "ui.h"
#include "utils.h"
#include <Windows.h>
#include "pdc_config.h" // IWYU pragma: keep
#include "../include/curses.h"

int main() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    start_color();
    curs_set(0);
    nodelay(stdscr, TRUE);
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

    GameData gameData = {NULL, 0, NULL};
    Timer timer;
    Currencies startCurrencies = {0};
    Resources startResources = {0};
    Relics startRelics = {0};
    float startPlayerLevel = 0.0f;
    int startPlayerExp = 0;

    Currencies currencies = {0};
    Resources resources = {0};
    Relics relics = {0};
    float playerLevel = 0.0f;
    int playerExp = 0;

    bool running = true;
    bool gameAttached = false;

    while (running) {
        if (!gameAttached) {
            if (initGameData(&gameData)) {
                initTimer(&timer);
                getCurrencies(&gameData, &startCurrencies);
                getResources(&gameData, &startResources);
                getRelics(&gameData, &startRelics);
                startPlayerLevel = getPlayerLevel(&gameData, startPlayerLevel);
                startPlayerExp = getPlayerExp(&gameData, startPlayerExp);
                gameAttached = true;
            } else {
                erase();
                printErrors(&gameData);
                refresh();
                int input = getch();
                if (input == 'q') {
                    running = false;
                }
                napms(1000);
                continue;
            }
        } else {
            erase();
            eraseErrors();
        }

        while (gameAttached && running) {
            char input = getch();
            if (input != ERR) {
                switch (input) {
                case 'q':
                    running = false;
                    break;
                case 'p':
                    togglePauseTimer(&timer);
                    break;
                }
            }

            erase();
            updateTimer(&timer);
            getCurrencies(&gameData, &currencies);
            getResources(&gameData, &resources);
            getRelics(&gameData, &relics);
            playerLevel = getPlayerLevel(&gameData, playerLevel);
            playerExp = getPlayerExp(&gameData, playerExp);
            drawUI(&timer, &startCurrencies, &startResources, &startRelics, startPlayerLevel, startPlayerExp, &currencies, &resources, &relics, playerLevel, playerExp);

            if (!IsWindow(gameData.hGameWindow)) {
                printError("Lost Game Window");
                gameAttached = false;
                if (gameData.processHandle != NULL) {
                    CloseHandle(gameData.processHandle);
                }
            }
        }
    }

    endwin();
    return EXIT_SUCCESS;
}
