#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdint.h>

#define MODULE_NAME "matrixListTest"
#define FUNC_NAME "testList"

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

    
    Py_DECREF(pValue);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    
    printf("We Made it here\n");
    // Py_ssize_t len = PyMapping_Size(pValue);
    // tData = (TrainData *)malloc(sizeof(TrainData) * len);
    // printf("Length of dict: %d\n", len);

    // PyObject *keys = PyMapping_Keys(pValue);
    // PyObject *items = PyMapping_Items(pValue);

    // if (!PyList_Check(keys) || !PyList_Check(items))
    // {
    //     printf("keys or items are not lists");
    // }

    // Py_ssize_t item_len = PyList_Size(items);
    // printf("Length of items: %lld\n", item_len);

    // for (Py_ssize_t i = 0; i < len; i++)
    // {
    //     PyObject *idObj = PyList_GetItem(keys, i);
    //     if (idObj == NULL)
    //     {
    //         printf("key not found at index: %lld\n", i);
    //         continue;
    //     }

    //     int64_t id = PyLong_AsLong(idObj);
    //     if (id < 0)
    //     {
    //         printf("id at index %lld - NaN\n", i);
    //         if (PyErr_Occurred())
    //             PyErr_Print();
    //         continue;
    //     }

    //     Py_DECREF(idObj);
    //     (tData + i)->id = id;
    //     // printf("Train id: %lld\n", id);

    //     PyObject *data = PyList_GetItem(items, i);
    //     if (data == NULL)
    //     {
    //         printf("list of data not found at index: %lld\n", i);
    //         continue;
    //     }
    //     else if (!PyTuple_Check(data))
    //     {
    //         printf("item is not a list at index: %lld\n", i);
    //         printf("Tuple? : %d, Long? : %d, Dicitonary? : %d\n", PyTuple_Check(data), PyLong_Check(data), PyMapping_Check(data));
    //         Py_DECREF(data);
    //         continue;
    //     }

    //     Py_ssize_t dataLen = PyMapping_Length(data);
    //     printf("data length: %lld\n", dataLen);

    //     (tData + i)->direction = getTrainData(data, 0);
    //     (tData + i)->nextStation = getTrainData(data, 1);
    //     (tData + i)->timeToStation = getTrainData(data, 2);
    //     (tData + i)->state = getTrainData(data, 3);

    //     Py_DECREF(data);
    // }

    // for (size_t i = 0; i < len; i++)
    // {
    //     printf("Train id: %d | ", (tData + i)->id);
    //     // printf("direction: %s |", (tData + i)->direction == -1 ? "Northbound" : "Southbound");
    //     printf("direction: %d | ", (tData + i)->direction);
    //     printf("%d station: %d", (tData + i)->state, (tData + i)->nextStation);
    //     // printf("%s station: %d", (tData + i)->state == 1 ? "next" : "current", (tData + i)->nextStation);
    //     printf("| time to station: %d\n", (tData + i)->timeToStation);
    // }

    // Py_DECREF(keys);
    // Py_DECREF(items);
    // Py_DECREF(pValue);
    // Py_XDECREF(pFunc);
    // Py_DECREF(pModule);



    if (Py_FinalizeEx() < 0)
        return 120;

    return 0;
}
