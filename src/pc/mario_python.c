
#include "mario_python.h"

#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"
#include "game/mario_step.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "object_python.h"
#include "game/interaction.h"

PyObject *gMarioModule;
extern struct MarioState *gMarioState;

/* = MarioState Object = */

struct _PyObjectClass;

typedef struct _PyMarioStateClass {
    PyObject_HEAD
    PyObject *native_state;
    struct _PyObjectClass *mario_object;
} PyMarioStateClass;

static PyMemberDef PyMarioState_members[] = {
    {"_native_state", T_OBJECT_EX, offsetof(PyMarioStateClass, native_state), 0, NULL},
    {"mario_object", T_OBJECT_EX, offsetof(PyMarioStateClass, mario_object), READONLY, NULL},
    {NULL}
};

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
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        mario_state->var = var; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET( var, type, c_getter ) \
    static PyObject * \
    PyMario_get_ ## var(const PyMarioStateClass *self) { \
        struct MarioState *mario_state = NULL; \
        PyObject *var; \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        var = c_getter(mario_state->var); \
        if (PyErr_Occurred()) { \
            fprintf( stderr, "during get " #var ":\n" ); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        return var; \
    }

#define MARIO_SET_VEC( var, type, py_fmt ) \
    static PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *args) { \
        struct MarioState *mario_state = NULL; \
        u32 idx = 0; \
        type val = 0; \
        int res = 0; \
        res = PyArg_ParseTuple(args, "l" #py_fmt, &idx, &val); \
        if (!res || PyErr_Occurred()) { \
            fprintf(stderr, "during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        mario_state->var[idx] = val; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET_VEC( var, type, c_getter ) \
    static PyObject * \
    PyMario_get_ ## var(PyMarioStateClass *self, PyObject *arg) { \
        struct MarioState *mario_state = NULL; \
        u32 idx = 0; \
        PyObject *var = 0; \
        idx = PyLong_AsLong(arg); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "mario: during get " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        var = c_getter(mario_state->var[idx]); \
        if (PyErr_Occurred()) { \
            fprintf( stderr, "mario: during get " #var ":\n" ); \
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

#ifndef CHECK_PYTHON

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

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

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

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

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

static PyObject *
PyMario_set_y_vel_based_on_fspeed(PyMarioStateClass *self, PyObject *args) {
    struct MarioState *mario_state = NULL;
    f32 initialVelY = 0;
    f32 multiplier = 0;
    int res = 0;

    res = PyArg_ParseTuple(args, "ff", &initialVelY, &multiplier);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during setVel:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

    // get_additive_y_vel_for_jumps is always 0 and a stubbed function.
    // It was likely trampoline related based on code location.
    mario_state->vel[1] = initialVelY + get_additive_y_vel_for_jumps() + mario_state->forwardVel * multiplier;

    if (mario_state->squishTimer != 0 || mario_state->quicksandDepth > 1.0f) {
        mario_state->vel[1] *= 0.5f;
    }
    
    Py_RETURN_NONE;
}

static PyObject *
PyMario_set_forwardVel_all(PyMarioStateClass *self, PyObject *arg) {
    struct MarioState *mario_state = NULL;
    f32 forwardVel = 0;
    
    forwardVel = PyFloat_AsDouble(arg);
    if (PyErr_Occurred()) {
        fprintf(stderr, "mario: during set forwardVel_all:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

    mario_state->forwardVel = forwardVel;

    mario_state->slideVelX = sins(mario_state->faceAngle[1]) * mario_state->forwardVel;
    mario_state->slideVelZ = coss(mario_state->faceAngle[1]) * mario_state->forwardVel;

    mario_state->vel[0] = (f32) mario_state->slideVelX;
    mario_state->vel[2] = (f32) mario_state->slideVelZ;

    Py_RETURN_NONE;
}

static PyObject *
PyMario_set_animID(PyMarioStateClass *self, PyObject *args) {
    struct MarioState *mario_state = NULL;
    s16 var = 0;

    var = PyLong_AsLong(args);
    if (PyErr_Occurred()) {
        fprintf(stderr, "mario: during set animID:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

    mario_state->marioObj->header.gfx.unk38.animID = var;

    Py_RETURN_NONE;
}

static PyObject *
PyMario_get_angle_to_object(PyMarioStateClass *self, struct _PyObjectClass *arg) {
    struct Object *obj = NULL;
    struct MarioState *mario_state = NULL;
    PyObject *pTanOut = NULL;
    f32 dx = 0.0f,
        dz = 0.0f;

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);
    obj = python_object_get_native(arg);

    dx = obj->oPosX - mario_state->pos[0];
    dz = obj->oPosZ - mario_state->pos[2];

    pTanOut = PyLong_FromLong(atan2s(dz, dx));

    return pTanOut;
}

#endif /* CHECK_PYTHON */

MARIO_SET( action, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( prevAction, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionArg, unsigned long, PyLong_AsUnsignedLong );
MARIO_SET( actionState, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( actionTimer, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( wallKickTimer, unsigned char, PyLong_AsUnsignedLong );
MARIO_SET( forwardVel, double, PyFloat_AsDouble );
MARIO_SET( peakHeight, double, PyFloat_AsDouble );
MARIO_SET( numCoins, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( healCounter, unsigned char, PyLong_AsUnsignedLong );
MARIO_SET_VEC( vel, float, "f" );
MARIO_SET_VEC( faceAngle, short, "h" );

MARIO_GET( flags, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( action, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( forwardVel, double, PyFloat_FromDouble );
MARIO_GET( intendedMag, double, PyFloat_FromDouble );
MARIO_GET( intendedYaw, short, PyLong_FromLong );
MARIO_GET( squishTimer, unsigned char, PyLong_FromLong );
MARIO_GET( quicksandDepth, double, PyFloat_FromDouble );
MARIO_GET( actionState, unsigned short, PyLong_FromUnsignedLong );
MARIO_GET( numCoins, unsigned short, PyLong_FromUnsignedLong );
MARIO_GET( healCounter, unsigned char, PyLong_FromUnsignedLong );
MARIO_GET_VEC( vel, float, PyFloat_FromDouble );
MARIO_GET_VEC( pos, float, PyFloat_FromDouble );
MARIO_GET_VEC( faceAngle, short, PyLong_FromLong );

static PyMethodDef PyMarioState_methods[] = {
    {"set_action",                  (PyCFunction)PyMario_set_action,                METH_O, NULL},
    {"set_action_state",            (PyCFunction)PyMario_set_actionState,           METH_O, NULL},
    {"set_prev_action",             (PyCFunction)PyMario_set_prevAction,            METH_O, NULL},
    {"set_action_timer",            (PyCFunction)PyMario_set_actionTimer,           METH_O, NULL},
    {"set_action_arg",              (PyCFunction)PyMario_set_actionArg,             METH_O, NULL},
    {"unset_flag",                  (PyCFunction)PyMario_unset_flag,                METH_O, NULL},
    {"set_flag",                    (PyCFunction)PyMario_set_flag,                  METH_O, NULL},
    {"set_forward_vel",             (PyCFunction)PyMario_set_forwardVel,            METH_O, NULL},
    {"set_wall_kick_timer",         (PyCFunction)PyMario_set_wallKickTimer,         METH_O, NULL},
    {"set_peak_height",             (PyCFunction)PyMario_set_peakHeight,            METH_O, NULL},
    {"set_heal_counter",            (PyCFunction)PyMario_set_healCounter,           METH_O, NULL},
    {"get_pos",                     (PyCFunction)PyMario_get_pos,                   METH_O, NULL},
    {"get_angle_to_object",         (PyCFunction)PyMario_get_angle_to_object,       METH_O, NULL},
    {"get_action",                  (PyCFunction)PyMario_get_action,                METH_NOARGS, NULL},
    {"get_forward_vel",             (PyCFunction)PyMario_get_forwardVel,            METH_NOARGS, NULL},
    {"get_intended_mag",            (PyCFunction)PyMario_get_intendedMag,           METH_NOARGS, NULL},
    {"get_intended_yaw",            (PyCFunction)PyMario_get_intendedYaw,           METH_NOARGS, NULL},
    {"get_squish_timer",            (PyCFunction)PyMario_get_squishTimer,           METH_NOARGS, NULL},
    {"get_quicksand_depth",         (PyCFunction)PyMario_get_quicksandDepth,        METH_NOARGS, NULL},
    {"set_vel",                     (PyCFunction)PyMario_set_vel,                   METH_VARARGS, NULL},
    {"get_vel",                     (PyCFunction)PyMario_get_vel,                   METH_O, NULL},
    {"get_num_coins",               (PyCFunction)PyMario_get_numCoins,              METH_NOARGS, NULL},
    {"set_num_coins",               (PyCFunction)PyMario_set_numCoins,              METH_O, NULL},
    {"set_face_angle",              (PyCFunction)PyMario_set_faceAngle,             METH_VARARGS, NULL},
    {"get_face_angle",              (PyCFunction)PyMario_get_faceAngle,             METH_O, NULL},
    {"get_flags",                   (PyCFunction)PyMario_get_flags,                 METH_NOARGS, NULL},
    {"get_action_state",            (PyCFunction)PyMario_get_actionState,           METH_NOARGS, NULL},
    #ifndef CHECK_PYTHON
    {"set_y_vel_based_on_fspeed",   (PyCFunction)PyMario_set_y_vel_based_on_fspeed, METH_VARARGS, NULL},
    {"set_forward_vel_all",         (PyCFunction)PyMario_set_forwardVel_all,        METH_O, NULL},
    {"set_anim_id",                 (PyCFunction)PyMario_set_animID,                METH_O, NULL},
    {"get_floor_class",             (PyCFunction)PyMario_get_floor_class,           METH_NOARGS, NULL},
    {"facing_downhill",             (PyCFunction)PyMario_facing_downhill,           METH_O, NULL},
    #endif /* !CHECK_PYTHON */
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
        fprintf(stderr, "type not ready?\n");
        return NULL;
    }

    pMario = PyModule_Create(&MarioModule);
    if(NULL == pMario) {
        fprintf(stderr, "could not allocate mario module\n");
        return NULL;
    }

    Py_INCREF(&PyMarioStateType);
    gMarioState->pyState = NULL;

    PYTHON_MARIO_ADD_LAYER_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_INPUT_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_STEP_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_PARTICLE_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_MARIO_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_ACTION_CONSTANTS(pMario);
    PYTHON_MARIO_ADD_INTERACTION_CONSTANTS(pMario);

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
        pMarioState->native_state = PYTHON_ENCAPSULE_MARIO(gMarioState, ;);
        pMarioState->mario_object = NULL;

        gMarioState->pyState = pMarioState;

        assert(PyCapsule_IsValid(pMarioState->native_state, "mario.MarioState._native_state"));
    }

    // Get rid of old Mario wrapper.
    Py_XDECREF(gMarioState->pyState->mario_object);

    pObject = object_python_wrap(gMarioState->marioObj);

    gMarioState->pyState->mario_object = (struct _PyObjectClass *)pObject;
    //Py_INCREF(gMarioState->pyState->mario_object);
    //Py_INCREF(gMarioState->pyState->mario_object->native_object);
    //assert(PyCapsule_IsValid(gMarioState->pyState->mario_object->native_object, "objects.Object._native_object"));

    assert(NULL != gMarioState->pyState->mario_object);

    fprintf(stdout, "new mario dropped\n");
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
