#include "t_renderer.h"

int main()
{
    trainsInit();

    TrainData *tData;

    tData = requestTrains();

    free(tData);
    trainsQuit();
    return 0;
}