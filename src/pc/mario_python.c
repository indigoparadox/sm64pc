
#include "mario_python.h"

#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"
#include "object_python.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

/* = MarioState Object = */

#define MARIO_SET( var, type, py_getter ) \
    static PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *args) { \
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
    PyMario_get_ ## var(PyMarioStateClass *self) { \
        PyObject *var; \
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

static PyObject *
PyMario_get_marioObj(PyObject *self) {
    PyMarioStateClass *pMario = (PyMarioStateClass *)self;
    assert(NULL != pMario->obj);
    return (PyObject *)(pMario->obj);
}

static PyMemberDef PyMarioState_members[] = {
    {"ptr", T_OBJECT_EX, offsetof(PyMarioStateClass, ptr), 0, NULL},
    {NULL}
};

MARIO_SET( action, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( prevAction, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionArg, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionState, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( actionTimer, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( forwardVel, double, PyFloat_AsDouble );

MARIO_GET( action, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( forwardVel, double, PyFloat_FromDouble );
MARIO_GET( intendedMag, double, PyFloat_FromDouble );

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
    {"get_mario_obj", PyMario_get_marioObj, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject PyMarioStateType = {
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

PyObject* PyInit_mario(void) {
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
    pMarioState = (PyMarioStateClass *)PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
    pMarioState->ptr = gMarioState;
    pMarioState->obj = NULL;
    PyModule_AddObject(pMario, "state", (PyObject *)pMarioState);

    gMarioState->pyState = pMarioState;

    #include "mario_python_actions.h"
    #include "mario_python_terrains.h"

    fprintf(stdout, "mario module initialized\n");

    return pMario;
}

void python_init_mario() {
    PyObject *pObject;

    Py_XDECREF(gMarioState->pyState->obj);

    pObject = object_python_wrap(gMarioState->marioObj);

    gMarioState->pyState->obj = (PyObjectClass *)pObject;
}

u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method) {
    PyObject *pFunc, *pArgs, *pValue;
    u32 retval = 0;

    pFunc = PyObject_GetAttrString(gMarioModule, method);
    if (pFunc && PyCallable_Check(pFunc)) {

        pArgs = PyTuple_New(3);
        Py_INCREF(m->pyState);
        PyTuple_SetItem(pArgs, 0, (PyObject *)m->pyState);
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
