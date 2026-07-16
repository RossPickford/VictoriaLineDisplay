#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdint.h>

typedef struct TrainData
{
    uint16_t id;
    int8_t direction;
    uint8_t timeToStation;
    uint8_t nextStation;
    uint8_t state;
} TrainData;

TrainData *tData;

uint64_t getTrainData(PyObject *data, Py_ssize_t index)
{
    if (!PyList_Check(data))
    {
        printf("This is not a list\n");
        return 0;
    }

    PyObject *pItem = PyList_GetItem(data, index);
    if (pItem == NULL)
    {
        printf("item inside item list not found at index: %lld\n", index);
        return 0;
    }

    int64_t item = PyLong_AsLong(pItem);
    Py_DECREF(pItem);

    if (item < 0)
    {
        printf("item at index %lld - NaN\n", index);
        if (PyErr_Occurred())
            PyErr_Print();
        return 0;
    }

    return item;
}

int main()
{
    // curl https://api.tfl.gov.uk/Line/victoria/Arrivals/940GZZLUOXC -o OxfordCircus.json

    PyObject *pName, *pModule, *pFunc;
    PyObject *pValue;
    int i;

    Py_Initialize();
    pName = PyUnicode_DecodeFSDefault("request");
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load\n");
        return 1;
    }

    pFunc = PyObject_GetAttrString(pModule, "getTrains");
    /* pFunc is a new reference */

    if (!pFunc && !PyCallable_Check(pFunc))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function\n");
    }

    pValue = PyObject_CallObject(pFunc, NULL);
    if (pValue == NULL || !PyMapping_Check(pValue))
    {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        if (pValue == NULL)
        {
            PyErr_Print();
            fprintf(stderr, "Call failed\n");
        }
        else
            printf("Object not a dictionary\n");

        return 1;
    }

    Py_ssize_t len = PyMapping_Size(pValue);
    tData = (TrainData *)malloc(sizeof(TrainData) * len);
    printf("Length of dict: %d\n", len);

    PyObject *keys = PyMapping_Keys(pValue);
    PyObject *items = PyMapping_Items(pValue);

    for (Py_ssize_t i = 0; i < len; i++)
    {
        PyObject *idObj = PyList_GetItem(keys, i);
        if (idObj == NULL)
        {
            printf("key not found at index: %lld\n", i);
            continue;
        }

        int64_t id = PyLong_AsLong(idObj);
        if (id < 0)
        {
            printf("id at index %lld - NaN\n", i);
            if (PyErr_Occurred())
                PyErr_Print();
            continue;
        }

        Py_DECREF(idObj);
        (tData + i)->id = id;
        // printf("Train id: %lld\n", id);

        PyObject *data = PyList_GetItem(items, i);
        if (data == NULL || !PyList_Check(data))
        {
            printf("list of data not found at index: %lld\n", i);
            continue;
        }

        (tData + i)->direction = getTrainData(data, 0);
        (tData + i)->nextStation = getTrainData(data, 1);
        (tData + i)->timeToStation = getTrainData(data, 2);
        (tData + i)->state = getTrainData(data, 3);

        Py_DECREF(data);
    }

    for (size_t i = 0; i < len; i++)
    {
        printf("Train id: %d |", (tData + i)->id);
        printf("direction: %s |", (tData + i)->direction == -1 ? "Northbound" : "Southbound");
        printf("%s station: %d", (tData + i)->state == 1 ? "next" : "current", (tData + i)->nextStation);
        printf("| time to station: %d\n", (tData + i)->timeToStation);
    }

    Py_DECREF(keys);
    Py_DECREF(pValue);
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);

    if (Py_FinalizeEx() < 0)
        return 120;

    return 0;
}
