
#include "mario_python.h"

#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"
#include "object_python.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

/* = MarioState Object = */

struct _PyObjectClass;

typedef struct _PyMarioStateClass {
    PyObject_HEAD
    PyObject *native_state;
    struct _PyObjectClass *mario_object;
} PyMarioStateClass;

#define MARIO_SET( var, type, py_getter ) \
    static PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *args) { \
        struct MarioState *mario_state = NULL; \
        type var; \
        var = py_getter(args); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "mario: during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PyCapsule_GetPointer(self->native_state, "mario.MarioState._native_state"); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "mario: during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state->var = var; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET( var, type, c_getter ) \
    static PyObject * \
    PyMario_get_ ## var(const PyMarioStateClass *self) { \
        struct MarioState *mario_state = NULL; \
        PyObject *var; \
        mario_state = PyCapsule_GetPointer(self->native_state, "mario.MarioState._native_state"); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "during get " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        var = c_getter(mario_state->var); \
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

static PyMemberDef PyMarioState_members[] = {
    {"_native_state", T_OBJECT_EX, offsetof(PyMarioStateClass, native_state), 0, NULL},
    {"mario_object", T_OBJECT_EX, offsetof(PyMarioStateClass, mario_object), READONLY, NULL},
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
PyMario_facing_downhill(const PyMarioStateClass *self, PyObject *arg) {
    s32 turnYaw;
    s32 res;
    PyObject *pRes;
    struct MarioState *mario_state = NULL;

    //assert(PyCapsule_IsValid(self->mario_object->native_object, "objects.Object._native_object"));
    assert(PyCapsule_IsValid(self->native_state, "mario.MarioState._native_state"));
    
    turnYaw = PyLong_AsLong( arg );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during facing_downhill:\n" );
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PyCapsule_GetPointer(self->native_state, "mario.MarioState._native_state");
    if (PyErr_Occurred()) {
        fprintf(stderr, "during facing_downhill:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    res = mario_facing_downhill(mario_state, turnYaw);
    pRes = PyLong_FromLong( res );
    if (PyErr_Occurred()) {
        fprintf(stderr, "during facing_downhill:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pRes;
}

static PyObject *
PyMario_get_floor_class(PyMarioStateClass *self) {
    s32 floorClass;
    PyObject *pRes;
    struct MarioState *mario_state = NULL;

    mario_state = PyCapsule_GetPointer(self->native_state, "mario.MarioState._native_state");
    if (PyErr_Occurred()) {
        fprintf(stderr, "during floor_class:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    //assert(PyCapsule_IsValid(self->mario_object->native_object, "objects.Object._native_object"));
    assert(PyCapsule_IsValid(self->native_state, "mario.MarioState._native_state"));
    
    floorClass = mario_get_floor_class(mario_state);
    
    pRes = PyLong_FromLong( floorClass );
    if (PyErr_Occurred()) {
        fprintf(stderr, "during floor_class:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    
    return pRes;
}

static PyMethodDef PyMarioState_methods[] = {
    {"set_action",          (PyCFunction)PyMario_set_action,         METH_O, NULL},
    {"set_action_state",    (PyCFunction)PyMario_set_actionState,    METH_O, NULL},
    {"set_prev_action",     (PyCFunction)PyMario_set_prevAction,     METH_O, NULL},
    {"set_action_timer",    (PyCFunction)PyMario_set_actionTimer,    METH_O, NULL},
    {"set_action_arg",      (PyCFunction)PyMario_set_actionArg,      METH_O, NULL},
    {"unset_flag",          (PyCFunction)PyMario_unset_flag,         METH_O, NULL},
    {"set_flag",            (PyCFunction)PyMario_set_flag,           METH_O, NULL},
    {"facing_downhill",     (PyCFunction)PyMario_facing_downhill,    METH_O, NULL},
    {"set_forward_vel",     (PyCFunction)PyMario_set_forwardVel,     METH_O, NULL},
    {"get_action",          (PyCFunction)PyMario_get_action,         METH_NOARGS, NULL},
    {"get_forward_vel",     (PyCFunction)PyMario_get_forwardVel,     METH_NOARGS, NULL},
    {"get_floor_class",     (PyCFunction)PyMario_get_floor_class,    METH_NOARGS, NULL},
    {"get_intended_mag",    (PyCFunction)PyMario_get_intendedMag,    METH_NOARGS, NULL},
    //{"get_mario_obj",       (PyCFunction)PyMario_get_marioObj,       METH_NOARGS, NULL},
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

static PyObject *
PyMario_random_float(PyObject *self) {
    f32 rnd_val = 0.0;
    PyObject *pRndVal = NULL;
    
    rnd_val = random_float();

    pRndVal = PyFloat_FromDouble( rnd_val );
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pRndVal;
}

static PyObject *
PyMario_random_ushort(PyObject *self) {
    u16 rnd_val = 0;
    PyObject *pRndVal = NULL;
    
    rnd_val = random_u16();

    pRndVal = PyLong_FromUnsignedLong( rnd_val );
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pRndVal;
}

static PyMethodDef PyMarioMethods[] = {
    {"random_float",    (PyCFunction)PyMario_random_float,    METH_NOARGS, NULL},
    {"random_ushort",   (PyCFunction)PyMario_random_ushort,   METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef MarioModule = {
    PyModuleDef_HEAD_INIT, "mario", NULL, -1, PyMarioMethods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_mario(void) {
    PyObject *pMario;
    //PyMarioStateClass *pMarioState;

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
    /*pMarioState = (PyMarioStateClass *)PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
    pMarioState->native_object = PyCapsule_New(gMarioState, "objects.Object._native_object");
    pMarioState->mario_object = NULL;
    PyModule_AddObject(pMario, "state", (PyObject *)pMarioState);*/

    gMarioState->pyState = NULL;

    #include "mario_python_actions.h"
    #include "mario_python_terrains.h"

    fprintf(stdout, "mario module initialized\n");

    return pMario;
}

void python_init_mario() {
    PyObject *pObject = NULL;
    PyMarioStateClass *pMarioState = NULL;

    if (NULL == gMarioState->pyState) {
        fprintf(stdout, "setting up mario python state...\n");
        Py_INCREF(&PyMarioStateType);
        pMarioState = (PyMarioStateClass *)PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
        pMarioState->native_state = PyCapsule_New(gMarioState, "mario.MarioState._native_state", NULL);
        pMarioState->mario_object = NULL;

        gMarioState->pyState = pMarioState;

        assert(PyCapsule_IsValid(pMarioState->native_state, "mario.MarioState._native_state"));
    }

    Py_XDECREF(gMarioState->pyState->mario_object);

    pObject = object_python_wrap(gMarioState->marioObj);

    gMarioState->pyState->mario_object = (struct _PyObjectClass *)pObject;
    Py_INCREF(gMarioState->pyState->mario_object);
    //Py_INCREF(gMarioState->pyState->mario_object->native_object);
    //assert(PyCapsule_IsValid(gMarioState->pyState->mario_object->native_object, "objects.Object._native_object"));

    assert(NULL != gMarioState->pyState->mario_object);
}

u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method) {
    PyObject *pFunc = NULL,
        *pArgs = NULL,
        *pValue = NULL,
        *pState = NULL,
        *pAction = NULL,
        *pActionArg = NULL;
    u32 retval = 0;

    assert(NULL != gMarioState->pyState->mario_object);
    assert(NULL != m->pyState->mario_object);

    //assert(PyCapsule_IsValid(m->pyState->mario_object->native_object, "objects.Object._native_object"));
    assert(PyCapsule_IsValid(m->pyState->native_state, "mario.MarioState._native_state"));

    pFunc = PyObject_GetAttrString(gMarioModule, method);
    if (pFunc && PyCallable_Check(pFunc)) {

        pArgs = PyTuple_New(3);
        
        /* The tuple will DECREF this for us later. */
        pState = (PyObject *)m->pyState;
        Py_INCREF(pState);
        PyTuple_SetItem(pArgs, 0, pState);
        
        /* The tuple will DECREF this for us later. */
        pAction = PyLong_FromUnsignedLong(action);
        PyTuple_SetItem(pArgs, 1, pAction);

        /* The tuple will DECREF this for us later. */
        pActionArg = PyLong_FromUnsignedLong(arg);
        PyTuple_SetItem(pArgs, 2, pActionArg);

        pValue = PyObject_CallObject(pFunc, pArgs);
        
        Py_XDECREF(pArgs);
        if(NULL != pValue) {
            retval = PyLong_AsLong(pValue);
            Py_DECREF(pValue);
        }
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return retval;
}
