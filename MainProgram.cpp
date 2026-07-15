#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

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

    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, "getTrains");
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc))
        {
            pValue = PyObject_CallObject(pFunc, NULL);
            if (pValue != NULL && PyMapping_Check(pValue))
            {
                printf("Result of call is a dictionary\n");
                Py_ssize_t len = PyMapping_Length(pValue);
                printf("Length of dict: %lld\n", len);
                PyObject *keys = PyMapping_Keys(pValue);
                for (Py_ssize_t i = 0; i < len; i++)
                {
                    PyObject *id = PyList_GetItem(keys, i);
                    uint64_t result = PyLong_AsLong(id);
                    if (result == -1)
                    {
                        fprintf(stderr, "result not converted");
                        return -1;
                    }
                    printf("Train id: %lld\n", result);
                }
                Py_DECREF(keys);
                Py_DECREF(pValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr, "Call failed\n");
                return 1;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function\n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "Failed to load\n");
        return 1;
    }
    if (Py_FinalizeEx() < 0)
    {
        return 120;
    }
    return 0;
}

// Why is this not working?

/*
Tested: PyRun_SimpleText works, the File is loaded correctly,
*/