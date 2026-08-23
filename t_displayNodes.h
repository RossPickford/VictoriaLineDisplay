#include "t_dataCollection.h"
#include <SDL3/SDL.h>
#include "t_stopData.h"

#pragma once

typedef struct trainNode
{
    float x;
    float speed;
    int16_t id;
    uint8_t misses;
    uint8_t nextStop;
    int8_t dir;
} trainNode;

bool t_nodes_init(trainNode **tNodes);
void t_nodes_quit(trainNode *tNodes);
void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t *tNodeLength, uint8_t tDataLength);
void updateTrainPosition(trainNode *tNodes, uint8_t tNodeLength, float deltaTime);
void testNodeDisplay(trainNode *tNodes, uint8_t *tNodeLen);
