#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdint.h>

#define MODULE_NAME "request"
#define FUNC_NAME "getTrains"

typedef struct TrainData
{
    uint16_t id;
    int8_t direction;
    uint8_t timeToStation;
    uint8_t nextStation;
    uint8_t state;
} TrainData;

TrainData *tData;

int64_t getTrainData(PyObject *data, Py_ssize_t index)
{
    PyObject *pItem = PyList_GetItem(data, index);
    if (pItem == NULL)
    {
        printf("item inside item list not found at index: %lld\n", index);
        return 0;
    }

    int64_t item = PyLong_AsLong(pItem);
    Py_DECREF(pItem);

    if (item < 0 && index != 1)
    {
        printf("item at index %lld - is less than zero\n", index);
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
    pName = PyUnicode_DecodeFSDefault(MODULE_NAME);

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL)
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load\n");
        return 1;
    }

    pFunc = PyObject_GetAttrString(pModule, FUNC_NAME);

    if (!pFunc && !PyCallable_Check(pFunc))
    {
        if (PyErr_Occurred())
            PyErr_Print();
        fprintf(stderr, "Cannot find function\n");
    }

    pValue = PyObject_CallObject(pFunc, NULL);
    if (pValue == NULL || !PyList_Check(pValue))
    {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        if (pValue == NULL)
        {
            PyErr_Print();
            fprintf(stderr, "Call failed\n");
        }
        else
            printf("Object not a list\n");

        return 1;
    }

    Py_ssize_t len = PyList_Size(pValue);
    tData = (TrainData *)malloc(sizeof(TrainData) * len);
    if (!tData)
    {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        Py_DECREF(pValue);
        fprintf(stderr, "failed to allocate memory\n");
        return -1;
    }

    printf("Length of list: %d\n", len);

    for (Py_ssize_t i = 0; i < len; i++)
    {
        PyObject *idObj = PyList_GetItem(pValue, i);
        if (idObj == NULL)
        {
            printf("key not found at index: %lld\n", i);
            continue;
        }
        else if (!PyList_Check(idObj))
        {
            Py_DECREF(idObj);
            printf("object is not a sub list at index: %lld\n", i);
            continue;
        }

        Py_ssize_t itemLen = PyList_Size(idObj);
        // printf("Length of internal array: %lld\n", itemLen);

        (tData + i)->id = getTrainData(idObj, 0);
        (tData + i)->direction = getTrainData(idObj, 1);
        (tData + i)->nextStation = getTrainData(idObj, 2);
        (tData + i)->timeToStation = getTrainData(idObj, 3);
        (tData + i)->state = getTrainData(idObj, 4);

        Py_DECREF(idObj);
    }

    free(tData);
    Py_DECREF(pValue);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);

    for (size_t i = 0; i < len; i++)
    {
        printf("Train id: %d | ", (tData + i)->id);
        printf("direction: %s |", (tData + i)->direction == -1 ? "Northbound" : "Southbound");
        printf("%s station: %d", (tData + i)->state == 1 ? "next" : "current", (tData + i)->nextStation);
        printf("| time to station: %d\n", (tData + i)->timeToStation);
    }

    if (Py_FinalizeEx() < 0)
        return 120;

    return 0;
}
