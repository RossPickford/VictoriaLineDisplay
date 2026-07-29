#include "t_displayNodes.h"
#include <math.h>

#define WALTHAMSTOW_BOUND 4
#define BRIXTON_BOUND 123

#define STATION_GAP 6

#define TNODE_ARENA_SIZE 50

int16_t tNodeOffset; // The offset starts from 0 - if it is below it means there are no nodse in the Arena

bool t_nodes_init(trainNode **tNodes)
{
    *tNodes = (trainNode *)SDL_malloc(sizeof(trainNode) * TNODE_ARENA_SIZE);

    if (!tNodes)
    {
        printf("failed to allocate memory for train nodes\n");
        return false;
    }

    for (size_t i = 0; i < TNODE_ARENA_SIZE; i++)
        (*tNodes + i)->misses = 0;

    tNodeOffset = -1;

    return true;
}

void t_nodes_quit(trainNode *tNodes)
{
    SDL_free(tNodes);
}

void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t tDataLength)
{
    // This only works under the assumption both are sorted in numerical order of IDs

    if (!tNodes || !tData)
    {
        printf("t_displayNodes: nodes or data pointer is Null - aborting function 'updateTrainNode'\n");
        return;
    }

    uint8_t node = 0, data = 0;
    while (node <= tNodeOffset && data < tDataLength)
    {
        trainNode tNode = *(tNodes + node);
        TrainData tInfo = *(tData + data);

        if (tNode.id == tInfo.id)
        {
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.speed = tInfo.timeToStation == 0 ? 0.0f : (double)((float)tNode.nextStop - tNode.x) / (double)tInfo.timeToStation;

            if ((tNode.speed < 0 && tNode.dir > 0) || ((tNode.speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
            {
                printf("t_displayNodes: Speed in the wrong direction due to next station given being behind train's current position\n");
                node++;
                data++;
                continue;
            }

            *(tNodes + node++) = tNode;
            data++;
        }
        else if (tNode.id < tInfo.id)
        {
            tNode.misses++;
            if (tNode.misses >= 3)
            {
                printf("t_displayNodes: Train %d did not receive data after 3 requests - removing node\n", tNode.id);

                size_t i = node;
                while (i < tNodeOffset)
                    *(tNodes + i++) = *(tNodes + i);

                tNodeOffset--;
            }
            else
                node++;
        }
        else // Insert a new node into the current position
        {
            if (tNodeOffset >= TNODE_ARENA_SIZE)
            {
                printf("Cannot assign another train node due to reaching Arena max - %u\n", tNodeOffset);
                data++;
                continue;
            }

            size_t i = ++tNodeOffset;
            while (i > node)
                *(tNodes + i--) = *(tNodes + i);

            tNode.id = tInfo.id;
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.x = (((float)tInfo.timeToStation / (float)tStops_times[dir][tInfo.nextStation]) * STATION_GAP * -tNode.dir) + tNode.nextStop;
            tNode.speed = tInfo.timeToStation == 0 ? 0.0f : (double)((float)tNode.nextStop - tNode.x) / (double)tInfo.timeToStation;

            if ((tNode.speed < 0 && tNode.dir > 0) || ((tNode.speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
            {
                printf("Speed in the wrong direction due to next station given being behind train's current position\n");
                node++;
                data++;
                continue;
            }

            *(tNodes + node++) = tNode;
            data++;
        }
    }

    // If there are no nodes, need to fill it with new data
    if (tNodeOffset <= 0)
        while (data < tDataLength)
        {
            tNodeOffset++;
            if (tNodeOffset >= TNODE_ARENA_SIZE)
            {
                printf("Cannot assign another train node due to reaching Arena max\n");
                tNodeOffset--;
                break;
            }

            trainNode tNode = *(tNodes + node);
            TrainData tInfo = *(tData + data);

            tNode.id = tInfo.id;
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.x = (((float)tInfo.timeToStation / (float)tStops_times[dir][tInfo.nextStation]) * STATION_GAP * (float)tNode.dir * -1.0f) + (float)tNode.nextStop;
            if (isnan(tNode.x))
            {
                tNode.x = (float)tNode.nextStop;
                // printf("t_displayNodes: x is Nan, %f / %f with index: %lu and %u \n", (float)tInfo.timeToStation, (float)tStops_times[dir][tInfo.nextStation], dir, tInfo.nextStation);
            }

            tNode.speed = tInfo.timeToStation == 0 ? 0.0f : (double)((float)tNode.nextStop - tNode.x) / (double)tInfo.timeToStation;
            if ((tNode.speed < 0 && tNode.dir > 0) || ((tNode.speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
            {
                printf("Speed in the wrong direction due to next station given being behind train's current position - %f : %d | x: %f stationpos: %u\n", tNode.speed, tNode.dir, tNode.x, tNode.nextStop);
                node++;
                data++;
                continue;
            }


            *(tNodes + node++) = tNode;
            data++;
        }
}

void updateTrainPosition(trainNode *tNodes, double deltaTime)
{
    if (tNodeOffset == 0)
    {
        printf("t_displayNodes: No current Trains Nodes\n");
        return;
    }
    else if (!tNodes)
    {
        printf("t_displayNodes: tNodes pointer Null\n");
        return;
    }
    else if (deltaTime == 0.0f)
    {
        // printf("t_displayNodes: delta time is: %f\n", deltaTime);
        return;
    }
    

    for (size_t i = 0; i < tNodeOffset; i++)
    {
        float xDelta = deltaTime * (tNodes + i)->speed /* * (tNodes + i)->dir */;
        (tNodes + i)->x += xDelta;

        if ((tNodes + i)->dir == NORTHBOUND && (tNodes + i)->x < (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
        else if ((tNodes + i)->dir == SOUTHBOUND && (tNodes + i)->x > (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
    }
}