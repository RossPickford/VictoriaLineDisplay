#include "t_dataCollection.h"
#include <stdlib.h>

#define DATA_ARENA_SIZE 50

static char readBuff[1024];

void t_data_init(TrainData **tData)
{
    *tData = (TrainData *)malloc(sizeof(TrainData) * DATA_ARENA_SIZE);
}

void t_data_quit(TrainData **tData)
{
    free(*tData);
}

void requestTrains(TrainData *tData, uint8_t *tDataLen)
{
    system("curl https://api.tfl.gov.uk/Line/victoria/Arrivals/ -o trains.txt");

    FILE *trains = fopen("trains.txt", "rb");

    if (!trains)
    {
        printf("failed to open files");
        return;
    }

    while (fgets(readBuff, sizeof(readBuff), trains))
    {
        printf("%s", readBuff);
    }

    fclose(trains);
}

int main(void)
{
    requestTrains(NULL, NULL);
    return 0;
}