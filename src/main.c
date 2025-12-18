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

    GameData gameData = {NULL, 0, NULL, NULL, NULL};
    Timer timer;
    Currencies startCurrencies;
    Resources startResources;
    Relics startRelics;
    float startPlayerLevel;
    int startPlayerExp;
    bool running = true;
    bool gameAttached = false;

    while (running) {
        if (!gameAttached) {
            if (initGameData(&gameData)) {
                initTimer(&timer);
                startCurrencies = getCurrencies(&gameData);
                startResources = getResources(&gameData);
                startRelics = getRelics(&gameData);
                startPlayerLevel = getPlayerLevel(&gameData);
                startPlayerExp = getPlayerExp(&gameData);
                gameAttached = true;
            } else {
                erase();
                writeString("   CANNOT READ GAME STATE   ", 12, 25, 5);
                writeString("  LAUNCH THE GAME TO ATTACH  ", 13, 25, 5);
                if (gameData.hGameWindow == NULL)
                    writeString("  UNABLE TO LOCATE WINDOW  ", 14, 25, 5);
                else if (gameData.pID == 0)
                    writeString("    UNABLE TO GET PID     ", 14, 25, 5);
                else if (gameData.processHandle == NULL)
                    writeString("   UNABLE TO GET HANDLE    ", 14, 25, 5);

                writeString("Press 'Q' to exit", 23, 2, 0);
                refresh();
                int input = getch();
                if (input == 'q') {
                    running = false;
                }
                napms(500);
                continue;
            }
        } else {
            erase();
            writeString("                            ", 12, 25, 0);
            writeString("                            ", 13, 25, 0);
            writeString("                            ", 14, 25, 0);
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
            drawUI(&gameData, &timer, &startCurrencies, &startResources, &startRelics, startPlayerLevel, startPlayerExp);

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
    if (gameData.monoModuleName != NULL) {
        free(gameData.monoModuleName);
    }
    if (gameData.unityModuleName != NULL) {
        free(gameData.unityModuleName);
    }
    return EXIT_SUCCESS;
}
