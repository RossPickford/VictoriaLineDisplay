#include "t_displayNodes.h"
#include <math.h>

#define WALTHAMSTOW_BOUND 4
#define BRIXTON_BOUND 123

#define TNODE_ARENA_SIZE 50

bool t_nodes_init(trainNode *tNodes)
{
    tNodes = SDL_malloc(sizeof(trainNode) * TNODE_ARENA_SIZE);

    if (!tNodes)
    {
        printf("failed to allocate memory for train nodes\n");
        return false;
    }

    for (size_t i = 0; i < TNODE_ARENA_SIZE; i++)
        (tNodes + i)->misses = 0;

    return true;
}

void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t tDataLength)
{
    // assume both are sorted in numerical order of IDs

    uint8_t node = 0, data = 0;
    while (node < tNodeOffset && data < tDataLength)
    {
        trainNode tNode = *(tNodes + node);
        TrainData tInfo = *(tData + data);

        if (tNode.id == tInfo.id)
        {
            tNode.dir = tInfo.direction;
            tNode.nextStop = tInfo.nextStation;
            tNode.speed = (float)(tNode.nextStop - tNode.x) / (float)tInfo.timeToStation;

            node++;
            data++;
        }
        else if (tNode.id < tInfo.id)
        {
            tNode.misses++;
            if (tNode.misses >= 3)
            {
                printf("Train %d did not receive data after 3 requests - removing node\n", tNode.id);

                size_t i = node;
                while (i < tNodeOffset)
                    *(tNodes + i++) = *(tNodes + i);

                tNodeOffset--;
            }
            else
                node++;
        }
        else
        {
            // Insert a new node into the current position 
        }
    }
}

void updateTrainPosition(trainNode *tNodes, uint64_t deltaTime)
{
    if (tNodeOffset == 0)
    {
        printf("No current Trains Nodes\n");
        return;
    }
    else if (!tNodes)
    {
        printf("tNodes pointer Null\n");
        return;
    }
    else if (deltaTime == 0)
    {
        printf("delta time is 0\n");
        return;
    }

    for (size_t i = 0; i < tNodeOffset; i++)
    {
        float xDelta = deltaTime * (tNodes + i)->speed * (tNodes + i)->dir;
        (tNodes + i)->x += xDelta;

        if ((tNodes + i)->dir == NORTHBOUND && (tNodes + i)->x < (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
        else if ((tNodes + i)->dir == SOUTHBOUND && (tNodes + i)->x > (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
    }
}