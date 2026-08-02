#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdint.h>

#pragma once

#define NORTHBOUND -1
#define SOUTHBOUND 1

typedef struct TrainData
{
    uint16_t id;
    int8_t direction;
    uint8_t timeToStation;
    uint8_t nextStation;
    uint8_t state;
} TrainData;

void t_data_init();
void t_data_quit();
TrainData *requestTrains(uint8_t *tDataLen);


