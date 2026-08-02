#include "t_displayNodes.h"
#include <math.h>

#define WALTHAMSTOW_BOUND 4
#define BRIXTON_BOUND 123

#define STATION_GAP 6

#define TNODE_ARENA_SIZE 50

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

    return true;
}

void t_nodes_quit(trainNode *tNodes)
{
    SDL_free(tNodes);
}

void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t *tNodeLength, uint8_t tDataLength)
{
    // This only works under the assumption both are sorted in numerical order of IDs

    printf("updating train nodes\n");

    if (!tNodes || !tData || !tNodeLength)
    {
        printf("t_displayNodes: nodes or data pointer is Null - aborting function 'updateTrainNode'\n");
        return;
    }

    uint8_t nodeLen = *tNodeLength; // saving my sanity in the stack
    uint8_t inode = 0, idata = 0;

    while (inode < nodeLen && idata < tDataLength)
    {
        trainNode tNode = *(tNodes + inode);
        TrainData tInfo = *(tData + idata);
        printf("tNodes[%u]: %u | tData[%u]: %u\n", inode, tNode.id, idata, tInfo.id);

        if (tNode.id == tInfo.id) // There is a match in data IDs
        {
            printf("t_displayNodes: changing single node data\n");
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.speed = tInfo.timeToStation == 0 ? 0.0f : (double)((float)tNode.nextStop - tNode.x) / (double)tInfo.timeToStation;

            if (tNode.speed > 1.0f)
                tNode.speed = 0.9f;
            else if (tNode.speed < -1.0f)
                tNode.speed = -0.9f;

            if ((tNode.speed < 0 && tNode.dir > 0) || ((tNode.speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
            {
                printf("t_displayNodes: Speed in the wrong direction due to next station given being behind train's current position\n");
                tNode.speed *= -1.0f;
            }
        }
        else if (tNode.id < tInfo.id) // There is no data for current node
        {
            tNode.misses++;
            printf("node missing data - %u\n", tNode.misses);

            if (tNode.misses >= 3)
            {
                printf("t_displayNodes: Train %d did not receive data after 3 requests - removing node\n", tNode.id);

                nodeLen--;
                for (size_t i = inode; i < nodeLen; i++)
                    *(tNodes + i) = *(tNodes + i + 1);
            }
            else if (inode < nodeLen)
                inode++;

            continue;
        }
        else if ((nodeLen + 1) < TNODE_ARENA_SIZE) // Insert a new node into the current position
        {
            printf("inserting new node\n");

            for (size_t i = nodeLen++; i > inode; i--)
            {
                printf("id before: %u\n", (tNodes + i)->id);
                *(tNodes + i) = *(tNodes + i - 1);
                printf("id after: %u\n", (tNodes + i)->id);
            }

            tNode.id = tInfo.id;
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.x = (((float)tInfo.timeToStation / (float)tStops_times[dir][tInfo.nextStation]) * STATION_GAP * -tNode.dir) + tNode.nextStop;
            tNode.x = (float)((int32_t)tNode.x);
            tNode.speed = tInfo.timeToStation == 0 ? 0.0f : (double)((float)tNode.nextStop - tNode.x) / (double)tInfo.timeToStation;

            if (tNode.speed > 1.0f)
                tNode.speed = 0.9f;
            else if (tNode.speed < -1.0f)
                tNode.speed = -0.9f;

            if ((tNode.speed < 0 && tNode.dir > 0) || ((tNode.speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
            {
                printf("t_displayNodes: Speed in the wrong direction due to next station given being behind train's current position\n");
                tNode.speed *= -1.0f;
            }
        }
        else
        {
            printf("Cannot assign another train node due to reaching Arena max - %u\n", nodeLen);
            break;
        }

        *(tNodes + inode) = tNode;
        if (inode < nodeLen)
            inode++;
        if (idata < tDataLength)
            idata++;
    }

    // If there are no nodes, need to fill it with new data
    if (idata < tDataLength)
        while (idata < tDataLength)
        {
            if (nodeLen >= TNODE_ARENA_SIZE)
            {
                printf("Cannot assign another train node due to reaching Arena max\n");
                break;
            }

            printf("t_displayNodes: filling node array\n");
            nodeLen++;

            trainNode tNode = *(tNodes + inode);
            TrainData tInfo = *(tData + idata++);

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
                tNode.speed *= -1.0f;
            }

            *(tNodes + inode++) = tNode;
        }

    *tNodeLength = nodeLen;
    printf("length: %u\n", *tNodeLength);
}

void updateTrainPosition(trainNode *tNodes, uint8_t tNodeLength, double deltaTime)
{
    printf("updating Pos\n");

    if (tNodeLength == 0)
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

    for (size_t i = 0; i < tNodeLength; i++)
    {
        float xDelta = deltaTime * (tNodes + i)->speed /* * (tNodes + i)->dir */;
        (tNodes + i)->x += xDelta;

        if ((tNodes + i)->dir == NORTHBOUND && (tNodes + i)->x < (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
        else if ((tNodes + i)->dir == SOUTHBOUND && (tNodes + i)->x > (tNodes + i)->nextStop)
            (tNodes + i)->x = (float)(tNodes + i)->nextStop;
    }
}