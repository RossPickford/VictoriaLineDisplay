#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "t_dataCollection.h"

#define MODULE_NAME "trainDataTest"
#define FUNC_NAME "requestTrainData"

#define DATA_ARENA_SIZE 50

// #define MODULE_NAME "request"
// #define FUNC_NAME "getTrains"

void t_data_init(TrainData **tData)
{
    Py_Initialize();

    *tData = (TrainData *)malloc(sizeof(TrainData) * DATA_ARENA_SIZE);
}

void t_data_quit(TrainData **tData)
{
    if (Py_FinalizeEx() < 0)
        ;

    free(*tData);
    // return 120;
}

int64_t getItem(PyObject *data, Py_ssize_t index)
{
    PyObject *pItem = PyList_GetItem(data, index);
    assert(pItem);

    int64_t item = PyLong_AsLong(pItem);
    Py_DECREF(pItem);

    if (item < 0 && index != 1)
    {
        printf("item at index %lld - is less than zero: %lld\n", index, item);
        return 0;
    }

    return item;
}

void requestTrains(TrainData *tData, uint8_t *tDataLen) // DO NOT CALL UNLESS Py_Initialize() HAS BEEN CALLED
{
    printf("requesting Data\n");

    assert(tData);
    *tDataLen = 0;

    PyObject *pName, *pModule, *pFunc;
    PyObject *pValue;

    pName = PyUnicode_DecodeFSDefault(MODULE_NAME);
    printf("retrieved name\n");

    pModule = PyImport_Import(pName);
    printf("retrieved module\n");
    Py_DECREF(pName);

    if (!pModule)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load\n");
        return;
    }

    pFunc = PyObject_GetAttrString(pModule, FUNC_NAME);

    if (!pFunc || !PyCallable_Check(pFunc))
    {
        PyErr_Print();
        fprintf(stderr, "Cannot find function\n");
    }
    printf("retrieved funciton\n");

    pValue = PyObject_CallObject(pFunc, NULL);
    printf("retrieved value/ called function\n");
    if (!pValue || !PyList_Check(pValue))
    {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        if (!pValue)
        {
            PyErr_Print();
            fprintf(stderr, "Call failed\n");
        }
        else
            printf("Object not a list\n");

        return;
    }

    *tDataLen = (uint8_t)PyList_Size(pValue);
    assert(*tDataLen <= DATA_ARENA_SIZE);
    printf("Length of list: %u\n", *tDataLen);

    for (Py_ssize_t i = 0; i < *tDataLen; i++)
    {
        PyObject *train = PyList_GetItem(pValue, i);
        if (!train)
        {
            printf("key not found at index: %lld\n", i);
            continue;
        }
        else if (!PyList_Check(train))
        {
            Py_DECREF(train);
            printf("object is not a sub list at index: %lld\n", i);
            continue;
        }

        // Py_ssize_t itemLen = PyList_Size(train);
        // printf("Length of internal array: %lld\n", itemLen);

        (tData + i)->id = (uint16_t)getItem(train, 0);
        assert((tData + i)->id >= 0);
        (tData + i)->direction = (int8_t)getItem(train, 1);
        assert((tData + i)->direction == -1 || (tData + i)->direction == 1);
        (tData + i)->nextStation = (uint8_t)getItem(train, 2);
        assert((tData + i)->nextStation >= 0 || (tData + i)->nextStation < 16);
        (tData + i)->timeToStation = (uint8_t)getItem(train, 3);
        (tData + i)->state = (uint8_t)getItem(train, 4);
    }

    printf("Now sorting Data\n");

    // sort the trains into numerical order of IDs

    for (size_t i = 0; i < (size_t)*tDataLen - 1; i++)
        for (size_t j = i + 1; j < *tDataLen; j++)
        {
            if ((tData + i)->id > (tData + j)->id)
            {
                TrainData tempData = *(tData + i);
                *(tData + i) = *(tData + j);
                *(tData + j) = tempData;
            }
        }

    Py_DECREF(pModule);
    Py_DECREF(pFunc);
    Py_DECREF(pValue);

    /* for (size_t i = 0; i < *tDataLen; i++)
    {
        printf("Train id: %d | ", (tData + i)->id);
        printf("direction: %s |", (tData + i)->direction == -1 ? "Northbound" : "Southbound");
        printf("%s station: %d", (tData + i)->state == 1 ? "next" : "current", (tData + i)->nextStation);
        printf("| time to station: %d\n", (tData + i)->timeToStation);
    } */
}