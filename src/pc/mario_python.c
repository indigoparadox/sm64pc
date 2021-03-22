

#include <Python.h>
#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

/* = MarioState Object = */

#define MARIO_SET( var, type, py_getter ) \
    static PyObject * \
    PyMario_set_ ## var(PyObject *self, PyObject *args) { \
        type var; \
        var = py_getter( args ); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        gMarioState->var = var; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET( var, type, c_getter ) \
    static PyObject * \
    PyMario_get_ ## var(PyObject *self) { \
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
        fprintf(stderr, "during unset flag:\n");
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
        fprintf(stderr, "during set flag:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    gMarioState->flags |= flag;

    Py_RETURN_NONE;
}

typedef struct {
    PyObject_HEAD
    int *ptr;
} PyMarioStateClass;

static PyMemberDef PyMarioState_members[] = {
    {"ptr", T_OBJECT_EX, offsetof(PyMarioStateClass, ptr), 0, NULL},
    {NULL}
};

MARIO_SET( action, unsigned long, PyLong_AsUnsignedLong );
MARIO_GET( action, unsigned long, PyLong_FromUnsignedLong );
MARIO_SET( prevAction, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionArg, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionState, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( actionTimer, unsigned short, PyLong_AsUnsignedLong );
MARIO_GET( forwardVel, unsigned long, PyLong_FromUnsignedLong );
MARIO_SET( forwardVel, unsigned short, PyLong_AsUnsignedLong );
MARIO_GET( intendedMag, unsigned short, PyLong_FromDouble );

static PyObject *
PyMario_facing_downhill(PyObject *self, PyObject *arg) {
    PyMarioStateClass *pm = (PyMarioStateClass *)self;
    s32 turnYaw;
    s32 res;
    PyObject *pRes;
    
    turnYaw = PyLong_AsLong( arg );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during facing_downhill:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }

    res = mario_facing_downhill(pm->ptr, turnYaw);
    pRes = PyLong_FromLong( res );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during facing_downhill:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pRes;
}

static PyObject *
PyMario_get_floor_class(PyObject *self) {
    PyMarioStateClass *pm = (PyMarioStateClass *)self;
    s32 floorClass;
    PyObject *pRes;
    
    floorClass = mario_get_floor_class(pm->ptr);
    
    pRes = PyLong_FromLong( floorClass );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during floor_class:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }
    
    return pRes;
}

static PyMethodDef PyMarioState_methods[] = {
    {"set_action", PyMario_set_action, METH_O, NULL},
    {"get_action", PyMario_get_action, METH_NOARGS, NULL},
    {"set_action_state", PyMario_set_actionState, METH_O, NULL},
    {"set_prev_action", PyMario_set_prevAction, METH_O, NULL},
    {"set_action_timer", PyMario_set_actionTimer, METH_O, NULL},
    {"set_action_arg", PyMario_set_actionArg, METH_O, NULL},
    {"unset_flag", PyMario_unset_flag, METH_O, NULL},
    {"set_flag", PyMario_set_flag, METH_O, NULL},
    {"facing_downhill", PyMario_facing_downhill, METH_O, NULL},
    {"set_forward_vel", PyMario_set_forwardVel, METH_O, NULL},
    {"get_forward_vel", PyMario_get_forwardVel, METH_NOARGS, NULL},
    {"get_floor_class", PyMario_get_floor_class, METH_NOARGS, NULL},
    {"get_intended_mag", PyMario_get_intendedMag, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject PyMarioStateType = {
    //PyVarObject_HEAD_INIT(&PyType_Type, 0)
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "mario.MarioState",
    .tp_basicsize = sizeof(PyMarioStateClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_methods = PyMarioState_methods,
    .tp_members = PyMarioState_members,
};

/* Mario Module */

static PyMethodDef PyMarioMethods[] = {
    {NULL, NULL, 0, NULL}
};

static PyModuleDef MarioModule = {
    PyModuleDef_HEAD_INIT, "mario", NULL, -1, PyMarioMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_mario(void) {
    PyObject *pMario;
    PyMarioStateClass *pMarioState;

    if(0 > PyType_Ready( &PyMarioStateType)) {
        fprintf( stderr, "type not ready?\n" );
        return NULL;
    }

    pMario = PyModule_Create(&MarioModule);
    if(NULL == pMario) {
        fprintf( stderr, "could not allocate mario module\n" );
        return NULL;
    }

    /*Py_INCREF(&PyMarioStateType);
    if( 0 > PyModule_AddObject(pMario, "MarioState", (PyObject *)&PyMarioStateType)) {
        Py_DECREF(&PyMarioStateType);
        Py_DECREF(pMario);
        return NULL;
    }*/

    Py_INCREF(&PyMarioStateType);
    pMarioState = PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
    pMarioState->ptr = gMarioState;
    PyModule_AddObject(pMario, "state", pMarioState);

    gMarioState->pyState = pMarioState;

    /*
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
    */

    #include "mario_python_actions.h"
    #include "mario_python_terrains.h"

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
    if (PyErr_Occurred()) {
        fprintf( stderr, "during setup:\n" );
        PyErr_Print();
    }

    assert( NULL != gMarioModule );

    fprintf(stdout, "mario module loaded\n");
}

u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method) {
    PyObject *pFunc, *pArgs, *pValue;
    u32 retval = 0;

    pFunc = PyObject_GetAttrString(gMarioModule, method);
    if (pFunc && PyCallable_Check(pFunc)) {

        pArgs = PyTuple_New(3);
        PyTuple_SetItem(pArgs, 0, m->pyState);
        pValue = PyLong_FromUnsignedLong( action );
        PyTuple_SetItem(pArgs, 1, pValue);
        pValue = PyLong_FromUnsignedLong( arg );
        PyTuple_SetItem(pArgs, 2, pValue);
        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        if(NULL != pValue) {
            retval = PyLong_AsLong( pValue );
            Py_DECREF(pValue);
        }
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return retval;
}
