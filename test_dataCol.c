#include "t_dataCollection.h"
#include <time.h>
#include <stdlib.h>

void t_data_init()
{
    Py_Initialize();
}

void t_data_quit()
{
    if (Py_FinalizeEx() < 0)
        ;
    // return 120;
}

uint8_t isCopy(uint16_t id, uint8_t index, TrainData *t, uint8_t tDataLen)
{
    for (uint8_t i = 0; i < tDataLen; i++)
        if (id == (t + i)->id && i != index)
            return 1;

    return 0;
}

/* TrainData *requestTrains(uint8_t *tDataLen) // random numerical values given
{
    // random and more complex
    uint8_t len = *tDataLen;
    srand(time(NULL) + len);

    len = (rand() % 6) + 5;

    TrainData *t = (TrainData *)malloc(sizeof(TrainData) * len);
    if (!t)
        printf("could not allocate memory for train data");

    for (uint8_t i = 0; i < len; i++)
    {
        (t + i)->id = rand() % (len + 5);
        while (isCopy((t + i)->id, i, t, len))
            (t + i)->id = rand() % (int)(len + 5);

        (t + i)->direction = ((rand() % 2) * 2) - 1;

        size_t x = ((t + i)->direction + 1) / 2;
        size_t y = rand() % 16;
        (t + i)->nextStation = tStops_pos[x][y];

        (t + i)->timeToStation = ((rand() % 2) * 101);
        (t + i)->state = 0;
    }

    for (size_t i = 0; i < len - 1; i++)
        for (size_t j = i + 1; j < len; j++)
        {
            if ((t + i)->id > (t + j)->id)
            {
                TrainData tempData = *(t + i);
                *(t + i) = *(t + j);
                *(t + j) = tempData;
            }
        }

    *tDataLen = len;

    return t;
} */

int count = 10;
TrainData *requestTrains(uint8_t *tDataLen) // add a new node each time
{
    // printf("requesting trains\n");

    if (count <= 0)
    {
        *tDataLen = 0;
        return NULL;
    }

    uint8_t len = *tDataLen;
    len = 1;
    srand(time(NULL));

    TrainData *t = (TrainData *)malloc(sizeof(TrainData) * len);
    if (!t)
        printf("could not allocate memory for train data");

    for (uint8_t i = 0; i < len; i++)
    {
        (t + i)->id = count--;

        (t + i)->direction = ((rand() % 2) * 2) - 1;
        (t + i)->nextStation = rand() % 16;
        (t + i)->timeToStation = rand() % 50;
        (t + i)->state = 0;
    }

    for (size_t i = 0; i < len - 1; i++)
        for (size_t j = i + 1; j < len; j++)
        {
            if ((t + i)->id > (t + j)->id)
            {
                TrainData tempData = *(t + i);
                *(t + i) = *(t + j);
                *(t + j) = tempData;
            }
        }

    *tDataLen = len;

    for (size_t i = 0; i < len; i++)
    {
        printf("Data id: %d | ", (t + i)->id);
        printf("direction: %d |", (t + i)->direction);
        printf("station: %d\n", (t + i)->nextStation);
    }

    return t;
}
