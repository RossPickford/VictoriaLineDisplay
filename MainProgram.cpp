// #define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    Py_Initialize();

    FILE *pyFile = fopen("request.py", "rb");
    if (pyFile == NULL)
    {
        printf("failed to load file");
        return -1;
    }

    PyRun_SimpleFile(pyFile, "request.py");

    if (Py_FinalizeEx() < 0)
        exit(120);

    fclose(pyFile);
    return 0;
}

// Why is this not working?

/*
Tested: PyRun_SimpleText works, the File is loaded correctly,
*/