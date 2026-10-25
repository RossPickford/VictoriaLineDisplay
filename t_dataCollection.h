#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#pragma once

#define NORTHBOUND -1
#define SOUTHBOUND 1

#define TRAIN_NEW 0
#define TRAIN_EXISTS 1

typedef enum station_name {
  V_NONE = 0,
  V_WALTHAMSTOW = 1,
  V_BLACKHORSE = 2,
  V_TOTTENHAM = 3,
  V_SEVENSISTERS = 4,
  V_FINSBURYPARK = 5,
  V_HIGHBURY = 6,
  V_KINGSCROSS = 7,
  V_EUSTON = 8,
  V_WARRENSTREET = 9,
  V_OXFORDCIRCUS = 10,
  V_GREENPARK = 11,
  V_VICTORIA = 12,
  V_PIMLICO = 13,
  V_VAUXHALL = 14,
  V_STOCKWELL = 15,
  V_BRIXTON = 16
} station_name;

typedef struct train_object {
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
  uint16_t timeToStation;

  /**
   * the station the train is approaching
   * collected from 'current location' field
   * The data within this field may not include the next station's name.
   * Instead it can contain the previous station name when describing leaving
   * it, or it may say 'at platform' - in which the 'station name' field is
   * required to get the next station name
   *
   * This variable represents the ordered numerical representation of each
   * station from 1 (Walthamstow Central) - 16 (Brixton).
   */
  station_name nextStation;

  /**
   * This is a simple bool that if true will change the nextStation variable
   * to the next station over - this is due to the key 'currentLocation' being
   * before 'Towards' and to get the next station over, you need to know the
   * direction.
   */
  uint8_t alterStation;

  /**
   * Status describes whether the train is new to the data pool or already
   * exists. 1 represents an existing train and 0 represents a new train
   */
  uint8_t status;

  /**
   * This is an eum that represents the perspective of the station the data
   * is being looked through. This is used to compare with the next station when
   * collecting the train data. They must be the same station for the Time to
   * Station value to be accurate.
   *
   * This should not be accessed outside of train data collection.
   */
  station_name stationName;

  /**
   * This is a bool that represents whether an existing train has had new data
   * collected from the most recent live stream - if not then it will be removed
   * from the array
   */
  uint8_t dataCollected;

  /**
   * The X position of the train on the actual LED screen
   */
  float x;

  /**
   * The calculated speed of the train - used to change the train's displayed
   * position
   */
  float speed;

} train_object;

typedef struct t_data {
  size_t len;
  train_object* data;
} t_data;

typedef struct String {
  size_t len;
  char* data;
} String;

void t_data_init(t_data* tdata);
void t_data_quit(t_data* tdata);
void parseTrains(t_data* tdata);
int isobjectempty(train_object data);
char* getstationliteral(station_name station);
float getSpeedf(train_object train);
void updateTrainPosition(t_data* tdata, float deltaTime);