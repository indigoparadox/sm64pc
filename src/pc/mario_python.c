

#include <Python.h>
#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

#define MARIO_SET( var, type, py_getter ) \
    static PyObject * \
    PyMario_set_ ## var(PyObject *self, PyObject *args) { \
        type var; \
        var = py_getter( args ); \
        if (PyErr_Occurred()) { \
            fprintf( stderr, "during set " #var ":\n" ); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        gMarioState->var = var; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET( var, type, c_getter ) \
    static PyObject * \
    PyMario_get_ ## var(PyObject *self, PyObject *args) { \
        type var; \
        var = c_getter( gMarioState->var ); \
        if (PyErr_Occurred()) { \
            fprintf( stderr, "during get " #var ":\n" ); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        return var; \
    }

static PyObject *
PyMario_unset_flag(PyObject *self, PyObject *arg) {
    unsigned long flag;

    flag = PyLong_AsUnsignedLong( arg );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during unset flag:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }
    gMarioState->flags &= ~flag;

    Py_RETURN_NONE;
}

static PyObject *
PyMario_set_flag(PyObject *self, PyObject *arg) {
    unsigned long flag;

    flag = PyLong_AsUnsignedLong( arg );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during set flag:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }
    gMarioState->flags |= flag;

    Py_RETURN_NONE;
}

MARIO_SET( action, unsigned long, PyLong_AsUnsignedLong );
MARIO_GET( action, unsigned long, PyLong_FromUnsignedLong );
MARIO_SET( prevAction, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionArg, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionState, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( actionTimer, unsigned short, PyLong_AsUnsignedLong );

static PyMethodDef MarioMethods[] = {
    {"set_action", PyMario_set_action, METH_O, NULL},
    {"get_action", PyMario_get_action, METH_NOARGS, NULL},
    {"set_action_state", PyMario_set_actionState, METH_O, NULL},
    {"set_prev_action", PyMario_set_prevAction, METH_O, NULL},
    {"set_action_timer", PyMario_set_actionTimer, METH_O, NULL},
    {"set_action_arg", PyMario_set_actionArg, METH_O, NULL},
    {"unset_flag", PyMario_unset_flag, METH_O, NULL},
    {"set_flag", PyMario_set_flag, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef MarioModule = {
    PyModuleDef_HEAD_INIT, "mario", NULL, -1, MarioMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_mario(void) {

    PyObject *pMario = PyModule_Create(&MarioModule);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_MASK", ACT_GROUP_MASK);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_STATIONARY", ACT_GROUP_STATIONARY);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_MOVING", ACT_GROUP_MOVING);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_AIRBORNE", ACT_GROUP_AIRBORNE);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_SUBMERGED", ACT_GROUP_SUBMERGED);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_CUTSCENE", ACT_GROUP_CUTSCENE);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_AUTOMATIC", ACT_GROUP_AUTOMATIC);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_OBJECT", ACT_GROUP_OBJECT);
    PyModule_AddIntConstant(pMario, "MARIO_ACTION_SOUND_PLAYED", MARIO_ACTION_SOUND_PLAYED);
    PyModule_AddIntConstant(pMario, "MARIO_MARIO_SOUND_PLAYED", MARIO_MARIO_SOUND_PLAYED);
    PyModule_AddIntConstant(pMario, "ACT_FLAG_AIR", ACT_FLAG_AIR);
    PyModule_AddIntConstant(pMario, "MARIO_UNKNOWN_18", MARIO_UNKNOWN_18);

    return pMario;
}

void python_init() {
    PyObject *pName, *pSysPath, *pCwd;
    char cwd[255];

    Py_SetProgramName( Py_DecodeLocale( "sm64pc", NULL ) );
    PyImport_AppendInittab("mario", &PyInit_mario);
    Py_Initialize();
    fprintf(stdout, "python initialized\n");

    /* Load the mario module. */
    /* TODO: Load from user dir? */
    pSysPath = PySys_GetObject((char*)"path");
    getcwd(cwd, 255);
    pCwd = PyUnicode_FromString(cwd);
    PyList_Append(pSysPath, pCwd);
    Py_DECREF(pCwd);
    pName = PyUnicode_DecodeFSDefault( "sm64" );
    /* TODO: Error checking of pName left out */

    gMarioModule = PyImport_Import( pName );

    assert( NULL != gMarioModule );

    fprintf(stdout, "mario module loaded\n");
}
