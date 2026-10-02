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

    /**
     * how many layers ofcurly braces the parser is currently inside.
     * It must be 1 to be considered inside a valid train item.
     */
    uint8_t braces = 0;

    /**
     *  This is a bi state variable. It will be 1 when the parser is considered inside a train item, and 0 when it is not
     */
    uint8_t trainItem = 0;

    while (fgets(readBuff, sizeof(readBuff), trains))
    {
        for (size_t i = 0; i < sizeof(readBuff); i++)
        {
            if (readBuff[i] == '{')
            {
                braces++;

                if (braces == 1)
                    trainItem = 1;
                else
                    trainItem = 0;
            }
            else if (readBuff[i] == '}')
            {
                if (braces > 0)
                    braces--;

                if (braces == 1)
                    trainItem = 1;
                else
                    trainItem = 0;
            }
            else if (trainItem == 1)
            {
                
            }
        }

        printf("%s", readBuff);
    }

    fclose(trains);
}

int main(void)
{
    requestTrains(NULL, NULL);
    return 0;
}