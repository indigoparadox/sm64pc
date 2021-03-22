

#include <Python.h>
#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

#define MARIO_SET( var, type ) \
    static PyObject * \
    PyMario_set_ ## var(PyObject *self, PyObject *args) { \
        type var; \
        if( PyArg_ParseTuple( args, ":" #var ) ) { \
            gMarioState->var = var; \
        } \
        if (PyErr_Occurred()) { \
            PyErr_Print(); \
        } \
        return NULL; \
    }

#define MARIO_GET( var, type ) \
    static PyObject * \
    PyMario_get_ ## var(PyObject *self, PyObject *args) { \
        fprintf( stdout, "get " #var ": %ld\n" ); \
        return PyLong_FromLong( gMarioState->var ); \
    }

static PyObject *
PyMario_unset_flag(PyObject *self, PyObject *args) {
    unsigned long flag;

    fprintf( stdout, "test2\n" );
    if( PyArg_ParseTuple( args, ":flag" ) ) {
        fprintf( stdout, "unset flag: %ld\n", flag );
        gMarioState->flags &= ~flag;
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }

    return NULL;
}

static PyObject *
PyMario_set_flag(PyObject *self, PyObject *args) {
    unsigned long flag;

    fprintf( stdout, "test1\n" );
    if( PyArg_ParseTuple( args, ":flag" ) ) {
        fprintf( stdout, "set flag: %ld\n", flag );
        gMarioState->flags |= flag;
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }

    return NULL;
}

/*
static PyObject *
PyMario_set_action(PyObject *self, PyObject *args) {
    unsigned long action;

    if( PyArg_ParseTuple( args, ":action" ) ) {
        gMarioState->action = action;
    }

    return NULL;
}
*/

MARIO_SET( action, unsigned long );
MARIO_GET( action, unsigned long );
MARIO_SET( prevAction, unsigned long );
MARIO_SET( actionArg, unsigned long );
MARIO_SET( actionState, unsigned short );
MARIO_SET( actionTimer, unsigned short );

static PyMethodDef MarioMethods[] = {
    {"set_action", PyMario_set_action, METH_VARARGS, NULL},
    {"get_action", PyMario_get_action, METH_VARARGS, NULL},
    {"set_action_state", PyMario_set_actionState, METH_VARARGS, NULL},
    {"set_prev_action", PyMario_set_prevAction, METH_VARARGS, NULL},
    {"set_action_timer", PyMario_set_actionTimer, METH_VARARGS, NULL},
    {"set_action_arg", PyMario_set_actionArg, METH_VARARGS, NULL},
    {"unset_flag", PyMario_unset_flag, METH_VARARGS, NULL},
    {"set_flag", PyMario_set_flag, METH_VARARGS, NULL},
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
