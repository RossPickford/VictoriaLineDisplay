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

uint8_t MissingNodeDataCheck(trainNode *tNodes, uint8_t nodeIndex, uint8_t *nodeLen)
{
    (tNodes + nodeIndex)->misses++;
    trainNode tNode = *(tNodes + nodeIndex);
    // printf("node missing data - %u\n", tNode.misses);

    if (tNode.misses >= 3)
    {
        // printf("t_displayNodes: Train %d did not receive data after 3 requests - removing node\n", tNode.id);

        (*nodeLen)--;
        for (size_t i = nodeIndex; i < *nodeLen; i++)
            *(tNodes + i) = *(tNodes + i + 1);

        return 0;
    }
    else
        return 1;
}

float getSpeedf(trainNode tNode, float time)
{
    float speed = time == 0 ? 0.0f : ((float)tNode.nextStop - tNode.x) / time;

    if (speed > 1.0f)
        speed = 0.9f;
    else if (speed < -1.0f)
        speed = -0.9f;

    if ((speed < 0 && tNode.dir > 0) || ((speed > 0 && tNode.dir < 0))) // check if direction of speed matches the train's direction.
    {
        printf("t_displayNodes: Speed in the wrong direction due to next station given being behind train's current position\n");
        speed *= -1.0f;
    }

    return speed;
}

void insertNewNode(trainNode *tNode, TrainData tData)
{
    tNode->id = tData.id;
    tNode->dir = tData.direction;
    size_t dir = (tNode->dir + 1) / 2;
    tNode->nextStop = tStops_pos[dir][tData.nextStation];
    tNode->x = (((float)tData.timeToStation / (float)tStops_times[dir][tData.nextStation]) * STATION_GAP * (float)tNode->dir * -1.0f) + (float)tNode->nextStop;

    if (isnan(tNode->x) || (((float)tNode->nextStop - tNode->x) * (float)tNode->dir) < 0.0f)
        tNode->x = (float)tNode->nextStop;

    if (tNode->x < WALTHAMSTOW_BOUND || tNode->x > BRIXTON_BOUND)
        tNode->x = (float)tNode->nextStop;

    tNode->x = (float)((int64_t)tNode->x);

    tNode->speed = getSpeedf(*tNode, (float)tData.timeToStation);
}

void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t *tNodeLength, uint8_t tDataLength)
{
    // This only works under the assumption both are sorted in numerical order of IDs

    // printf("updating train nodes\n");

    if (!tNodes || !tNodeLength)
    {
        printf("t_displayNodes: nodes or node length is Null - aborting function 'updateTrainNode'\n");
        return;
    }
    else if (!tData)
    {
        for (size_t i = 0; i < *tNodeLength; i += MissingNodeDataCheck(tNodes, i, tNodeLength))
            ;
        return;
    }

    uint8_t nodeLen = *tNodeLength; // saving my sanity in the stack
    uint8_t inode = 0, idata = 0;

    printf("node length: %u || data length: %u\n", *tNodeLength, tDataLength);
    while (inode < nodeLen && idata < tDataLength)
    {
        trainNode tNode = *(tNodes + inode);
        TrainData tInfo = *(tData + idata);
        printf("tNodes[%u]: %u | tData[%u]: %u\n", inode, tNode.id, idata, tInfo.id);

        if (tNode.id == tInfo.id) // There is a match in data IDs
        {
            // printf("t_displayNodes: changing single node data\n");
            tNode.dir = tInfo.direction;
            size_t dir = (tNode.dir + 1) / 2;
            tNode.nextStop = tStops_pos[dir][tInfo.nextStation];
            tNode.speed = getSpeedf(tNode, (float)tInfo.timeToStation);
            tNode.misses = 0;
        }
        else if (tNode.id < tInfo.id) // There is no data for current node
        {
            inode += MissingNodeDataCheck(tNodes, inode, &nodeLen);
            continue;
        }
        else if ((nodeLen + 1) < TNODE_ARENA_SIZE) // Insert a new node into the current position
        {
            // printf("inserting new node\n");

            for (size_t i = nodeLen++; i > inode; i--)
                *(tNodes + i) = *(tNodes + i - 1);

            insertNewNode(&tNode, tInfo);
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

    if (inode < nodeLen)
        while (inode < nodeLen)
            inode += MissingNodeDataCheck(tNodes, inode, &nodeLen);

    // If there are no nodes, need to fill it with new data
    if (idata < tDataLength)
        while (idata < tDataLength)
        {
            if (nodeLen >= TNODE_ARENA_SIZE)
            {
                printf("Cannot assign another train node due to reaching Arena max\n");
                break;
            }

            // printf("t_displayNodes: filling node array\n");
            nodeLen++;

            insertNewNode((tNodes + inode++), *(tData + idata++));
        }

    *tNodeLength = nodeLen;
}

void updateTrainPosition(trainNode *tNodes, uint8_t tNodeLength, float deltaTime)
{
    // printf("updating Pos\n");

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
        trainNode tNode = *(tNodes + i);
        float xDelta = deltaTime * tNode.speed /* * (tNodes + i)->dir */;
        tNode.x += xDelta;

        if (tNode.dir == NORTHBOUND && tNode.x < tNode.nextStop)
        {
            tNode.x = (float)tNode.nextStop;
            tNode.speed = 0.0f;
        }
        else if (tNode.dir == SOUTHBOUND && tNode.x > tNode.nextStop)
            tNode.x = (float)tNode.nextStop;

        *(tNodes + i) = tNode;
    }
}