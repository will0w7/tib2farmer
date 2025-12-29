#include "timer.h"

void initTimer(Timer* timer) {
    timer->startTime = time(NULL);
    timer->pauseStartTime = time(NULL);
    timer->paused = true;
    timer->elapsedSeconds = 0;
}

void updateTimer(Timer* timer) {
    time_t nowTime = time(NULL);
    if (!timer->paused) {
        timer->elapsedSeconds = (int)difftime(nowTime, timer->startTime);
    }
}

void togglePauseTimer(Timer* timer) {
    timer->paused = !timer->paused;
    if (timer->paused) {
        timer->pauseStartTime = time(NULL);
    } else {
        timer->startTime += difftime(time(NULL), timer->pauseStartTime);
    }
}
