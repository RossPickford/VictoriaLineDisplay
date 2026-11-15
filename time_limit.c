#include <stdio.h>
#include <time.h>

#define REQUEST_TIME 30

int main(void) {
  float previousTick = 0.0f, timeelapsed = 0.0f;

  while (1) {
    clock_t tick = clock();
    float currentTick = (float)tick / (float)CLOCKS_PER_SEC; // convert from ms to seconds
    float delta = currentTick - previousTick;  
    previousTick = currentTick;

    timeelapsed += delta;

    printf("current tick: %lu | current tick as float: %f | delta: %f | timeelapsed: %f\n", tick, currentTick, delta, timeelapsed);

    if (timeelapsed >= REQUEST_TIME) {
        timeelapsed = (float)((int)timeelapsed % REQUEST_TIME);
        printf("\n====== time has elapsed ======\n");
    }
  }
  return 0;
}