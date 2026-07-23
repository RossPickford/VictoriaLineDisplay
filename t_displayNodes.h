#include "t_dataCollection.h"
#include <SDL3/SDL.h>

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

typedef struct trainNode
{
    int8_t dir;
    uint16_t id;
    float x;
    float speed;
    TrainStops nextStop;
} trainNode;

bool updateTrainNode(trainNode *tNodes, TrainData *tData);
bool updateTrainPosition(trainNode *tNodes, uint64_t curTime);
trainNode *getTrainNodes();