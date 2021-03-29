
#include "level_python.h"

#include "game/level_update.h"
#include "course_table.h"

extern s16 gCurrCourseNum;

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

static PyObject *
PyLevel_get_curr_course_num(PyObject *self) {
    PyObject *pCurrCourseNum = NULL;

    pCurrCourseNum = PyLong_FromLong(gCurrCourseNum);
    if (PyErr_Occurred()) {
        fprintf(stderr, "during get curr save file num:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pCurrCourseNum;
}

#endif /* CHECK_PYTHON */

static PyMethodDef PyLevels_methods[] = {
    #ifndef CHECK_PYTHON
    {"initiate_warp",   (PyCFunction)PyLevel_initiate_warp,   METH_VARARGS, NULL},
    {"get_curr_course_num",   (PyCFunction)PyLevel_get_curr_course_num,   METH_NOARGS, NULL},
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

    PYTHON_LEVEL_ADD_WARP_CONSTANTS(pLevels);
    PYTHON_LEVEL_ADD_TERRAIN_CONSTANTS(pLevels);
    PYTHON_LEVEL_ADD_SURFACE_CONSTANTS(pLevels);
    PYTHON_LEVEL_ADD_LEVEL_CONSTANTS(pLevels);

    PyModule_AddIntConstant(pLevels, "COURSE_MIN", COURSE_MIN);
    PyModule_AddIntConstant(pLevels, "COURSE_MAX", COURSE_MAX);
    PyModule_AddIntConstant(pLevels, "COURSE_STAGES_MAX", COURSE_STAGES_MAX);

    fprintf(stdout, "levels module initialized\n");

    return pLevels;
}
