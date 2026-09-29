#include "t_dataCollection.h"
#include <stdlib.h>

#define DATA_ARENA_SIZE 50

t_data_init(TrainData **tData)
{
    *tData = (TrainData *)malloc(sizeof(TrainData) * DATA_ARENA_SIZE);
}

t_data_quit(TrainData **tData)
{
    free(*tData);
}

requestTains(TrainData *tData, uint8_t *tDataLen)
{
    system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.json");

    FILE *trains = fopen("northbound.json", "r");

    if (!trains)
    {
        printf("failed to open files");
        return -1;
    }

    fclose(trains);
}