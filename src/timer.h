#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <stdbool.h>

typedef struct {
    time_t startTime;
    time_t pauseStartTime;
    bool paused;
    int elapsedSeconds;
} Timer;

void initTimer(Timer* timer);
void updateTimer(Timer* timer);
void togglePauseTimer(Timer* timer);

#endif
