#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdint.h>

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

void trainsInit();
void trainsQuit();
int64_t getItem(PyObject *data, Py_ssize_t index);
TrainData *requestTrains();


