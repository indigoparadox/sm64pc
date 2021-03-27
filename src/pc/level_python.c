
#include "level_python.h"

#include "level_python_levels.h"
#include "game/level_update.h"

/* Levels Module */

#ifndef CHECK_PYTHON

static PyObject *
PyLevel_initiate_warp(PyObject *self, PyObject *args) {
    s16 destLevel = 0,
        destArea = 0,
        destWarpNode = 0;
    s32 arg3 = 0;
    int res = 0;

    res = PyArg_ParseTuple(args, "hhhl", &destLevel, &destArea, &destWarpNode, &arg3);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during warp:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    initiate_warp(destLevel, destArea, destWarpNode, arg3);

    Py_RETURN_NONE;
}

#endif /* CHECK_PYTHON */

static PyMethodDef PyLevels_methods[] = {
    #ifndef CHECK_PYTHON
    {"initiate_warp",   (PyCFunction)PyLevel_initiate_warp,   METH_VARARGS, NULL},
    #endif /* CHECK_PYTHON */
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PyLevelsModule = {
    PyModuleDef_HEAD_INIT, "levels", NULL, -1, PyLevels_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_levels(void) {
    PyObject *pLevels;

    pLevels = PyModule_Create(&PyLevelsModule);
    if(NULL == pLevels) {
        fprintf( stderr, "could not allocate levels module\n" );
        return NULL;
    }

    ADD_LEVEL_CONSTANTS(pLevels);

    fprintf(stdout, "levels module initialized\n");

    return pLevels;
}
