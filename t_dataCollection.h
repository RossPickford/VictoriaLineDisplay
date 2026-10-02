#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#pragma once

#define NORTHBOUND -1
#define SOUTHBOUND 1

typedef struct TrainData
{
    /**
     * vehicle id of the train - it is unique to each train
     * collected from 'vehicle id' field
     */
    uint16_t id; 

    /**
     * this is either Northbound or Southbound
     * The data is collected from 'towards' field
     */
    int8_t direction;

    /**
     * the time in seconds to the next station
     * collected from 'timeToStation' field
     * 
     * If the 'current location' field states that a train is 'at' a station -
     * then this variable can be set to 0
     */
    uint8_t timeToStation;

    /**
     * the station the train is approaching
     * collected from 'current location' field
     * The data within this field may not include the next station's name.
     * Instead it can contain the previous station name when describing leaving it,
     * or it may say 'at platform' - in which the 'station name' field is required to
     * get the next station name
     * 
     * This variable represents the ordered numerical representation of each station 
     * from 0 (Walthamstow Central) - 15 (Brixton).
     */
    uint8_t nextStation;

    /**
     * State describes whether the train is moving or stationary
     * 1 represents a moving train and 0 represents a stationary train
     */
    uint8_t state;

    /**
     * This is a variable represents the perspective of the station that the data is being looked through.
     * This is used to compare with the next station when collecting the train data. They must be the same
     * station for the Time to Station value to be accurate.
     * 
     * This should not be accessed outside of train data collection.
     */
    uint8_t stationName;
    
} TrainData;

void t_data_init(TrainData ** tData);
void t_data_quit(TrainData **tData);
void requestTrains(TrainData *tData, uint8_t *tDataLen);


