#include "t_dataCollection.h"
#include <SDL3/SDL.h>

#pragma once

typedef enum TrainStops
{
    Southbound_Waltham = 4,
    Southbound_Blackhorse = 12,
    Southbound_Tottenham = 20,
    Southbound_Seven = 28,
    Southbound_Finsbury = 36,
    Southbound_Highbury = 44,
    Southbound_Kings = 52,
    Southbound_Euston = 60,
    Southbound_Warren = 68,
    Southbound_Oxford = 76,
    Southbound_Green = 84,
    Southbound_Victoria = 92,
    Southbound_Pimlico = 100,
    Southbound_Vauxhall = 108,
    Southbound_Stockwell = 116,
    Southbound_Brixton = 124,

    Northbound_Waltham = 3,
    Northbound_Blackhorse = 11,
    Northbound_Tottenham = 19,
    Northbound_Seven = 27,
    Northbound_Finsbury = 35,
    Northbound_Highbury = 43,
    Northbound_Kings = 51,
    Northbound_Euston = 59,
    Northbound_Warren = 67,
    Northbound_Oxford = 75,
    Northbound_Green = 83,
    Northbound_Victoria = 91,
    Northbound_Pimlico = 99,
    Northbound_Vauxhall = 107,
    Northbound_Stockwell = 115,
    Northbound_Brixton = 123,
} TrainStops;

static uint8_t tStops_pos[2][16] = {
    {
        // Northbound //
        3,   // Walthamstow
        11,  // Blackhorse road
        19,  // Tottenham Hale
        27,  // Seven Sisters
        35,  // Finsbury Park
        43,  // Highbury & Islington
        51,  // Kings Cross st. Pancras
        59,  // Euston
        67,  // Warren Street
        75,  // Oxford Circus
        83,  // Green Park
        91,  // Victoria
        99,  // Pimlico
        107, // Vauxhall
        115, // Stockwell
        123, // Brixton
    },
    {
        // Southbound //
        4,   // Walthamstow
        12,  // Blackhorse road
        20,  // Tottenham Hale
        28,  // Seven Sisters
        36,  // Finsbury Park
        44,  // Highbury & Islington
        52,  // Kings Cross st. Pancras
        60,  // Euston
        68,  // Warren Street
        76,  // Oxford Circus
        84,  // Green Park
        92,  // Victoria
        100, // Pimlico
        108, // Vauxhall
        116, // Stockwell
        124, // Brixton
    }};

static uint8_t tStops_times[2][16] = {
    {
        // Northbound //
        105, // Walthamstow
        110, // Blackhorse road
        100, // Tottenham Hale
        205, // Seven Sisters
        145, // Finsbury Park
        165, // Highbury & Islington
        90,  // Kings Cross st. Pancras
        90,  // Euston
        100, // Warren Street
        120, // Oxford Circus
        85,  // Green Park
        120, // Victoria
        85,  // Pimlico
        130, // Vauxhall
        120, // Stockwell
        0,   // Brixton
    },
    {
        // Southbound //
        0,   // Walthamstow
        125, // Blackhorse road
        115, // Tottenham Hale
        110, // Seven Sisters
        200, // Finsbury Park
        140, // Highbury & Islington
        175, // Kings Cross st. Pancras
        90,  // Euston
        90,  // Warren Street
        95,  // Oxford Circus
        115, // Green Park
        115, // Victoria
        110, // Pimlico
        90,  // Vauxhall
        135, // Stockwell
        105, // Brixton
    }};

typedef struct trainNode
{
    uint8_t misses;
    int8_t dir;
    uint16_t id;
    float x;
    double speed;
    TrainStops nextStop;
} trainNode;

bool t_nodes_init(trainNode **tNodes);
void t_nodes_quit(trainNode *tNodes);
void updateTrainNode(trainNode *tNodes, TrainData *tData, uint8_t tDataLength);
void updateTrainPosition(trainNode *tNodes, uint64_t deltaTime);
trainNode *getTrainNodes();