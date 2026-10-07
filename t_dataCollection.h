#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#pragma once

#define NORTHBOUND -1
#define SOUTHBOUND 1

typedef enum station_name {
  V_WALTHAMSTOW = 0,
  V_BLACKHORSE = 1,
  V_TOTTENHAM = 2,
  V_SEVENSISTERS = 3,
  V_FINSBURYPARK = 4,
  V_HIGHBURY = 5,
  V_KINGSCROSS = 6,
  V_EUSTON = 7,
  V_WARRENSTREET = 8,
  V_OXFORDCIRCUS = 9,
  V_GREENPARK = 10,
  V_VICTORIA = 11,
  V_PIMLICO = 12,
  V_VAUXHALL = 13,
  V_STOCKWELL = 14,
  V_BRIXTON = 15
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
  uint8_t timeToStation;

  /**
   * the station the train is approaching
   * collected from 'current location' field
   * The data within this field may not include the next station's name.
   * Instead it can contain the previous station name when describing leaving
   * it, or it may say 'at platform' - in which the 'station name' field is
   * required to get the next station name
   *
   * This variable represents the ordered numerical representation of each
   * station from 0 (Walthamstow Central) - 15 (Brixton).
   */
  uint8_t nextStation;

  /**
   * State describes whether the train is moving or stationary
   * 1 represents a moving train and 0 represents a stationary train
   */
  uint8_t state;

  /**
   * This is an eum that represents the perspective of the station the data
   * is being looked through. This is used to compare with the next station when
   * collecting the train data. They must be the same station for the Time to
   * Station value to be accurate.
   *
   * This should not be accessed outside of train data collection.
   */
  station_name stationName;

} train_object;

typedef struct t_data {
  size_t len;
  train_object* data;
} t_data;

typedef enum KeyType {
  KT_NONE,
  KT_ID,
  KT_STATIONNAME,
  KT_TIMETOSTATION,
  KT_CURRENTLOCATION,
  KT_TOWARDS,
  KT_LIMBO
} KeyType;

typedef struct String {
  size_t len;
  char* data;
} String;

// void t_data_init(train_object ** tData);
// void t_data_quit(train_object **tData);
// void requestTrains(train_object *tData, uint8_t *tDataLen);
void t_data_init(t_data* tdata);
void t_data_quit(t_data* tdata);
void requestTrains(t_data* tdata);
