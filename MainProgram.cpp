#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

int
main(int argc, char* argv[])
{
    Py_Initialize();

    FILE* pyFile = fopen("request.py", "r");
    PyRun_SimpleFile(pyFile, "request.py");

    return 0;
}
