#include "t_displayNodes.h"
#include <math.h>

#define WALTHAMSTOW_BOUND 4
#define BRIXTON_BOUND 123

bool updateTrainNode(trainNode *tNodes, TrainData *tData)
{
    //assume both are sorted in numerical order of IDs
}

void updateTrainPosition(trainNode *tNodes, size_t size, uint64_t deltaTime)
{
    if (size <= 0)
    {
        printf("No current Trains Nodes\n");
        return;
    }
    else if (!tNodes)
    {
        printf("tNodes pointer Null\n");
        return;
    }
    else if(deltaTime <= 0)
    {
        printf("delta time is 0\n");
        return;
    }

    for (size_t i = 0; i < size; i++)
    {
        float xDelta = deltaTime * (tNodes + i)->speed * (tNodes + i)->dir;
        (tNodes + i)->x += xDelta;

        if ((tNodes + i)->dir == NORTHBOUND && (tNodes + i)->x < (tNodes + i)->nextStop)
                (tNodes + i)->x = (float)(tNodes + i)->nextStop;
        else if ((tNodes +i)->dir == SOUTHBOUND && (tNodes + i)->x > (tNodes + i)->nextStop)
            (tNodes +i)->x = (float)(tNodes + i)->nextStop;
    }
}