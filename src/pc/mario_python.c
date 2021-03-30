
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
#include "logging_python.h"

static PyObject *sLogger = NULL;
PyObject *gMarioModule;
extern struct MarioState *gMarioState;
extern u32 gGlobalTimer;

void reset_mario_pitch(struct MarioState *);
void push_mario_out_of_object(struct MarioState *, struct Object *, f32);
u32 bully_knock_back_mario(struct MarioState *);
u32 check_read_sign(struct MarioState *m, struct Object *o);
u32 check_npc_talk(struct MarioState *m, struct Object *o);
void check_kick_or_punch_wall(struct MarioState *m);

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

#define MARIO_SET(var, type, py_getter) \
    static PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *args) { \
        struct MarioState *mario_state = NULL; \
        type var; \
        var = (type)py_getter(args); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "mario: during set " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        mario_state->var = (type)var; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET(var, type, c_getter) \
    static PyObject * \
    PyMario_get_ ## var(const PyMarioStateClass *self) { \
        struct MarioState *mario_state = NULL; \
        PyObject *var; \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        var = c_getter(mario_state->var); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "during get " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        return var; \
    }

#define MARIO_SET_VEC(var, type, py_fmt) \
    static PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *args) { \
        struct MarioState *mario_state = NULL; \
        u32 idx = 0; \
        type val = 0; \
        int res = 0; \
        res = PyArg_ParseTuple(args, "l" #py_fmt, &idx, &val); \
        if (!res || PyErr_Occurred()) { \
            python_log_error(sLogger, "during set " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        mario_state->var[idx] = val; \
        Py_RETURN_NONE; \
    }

#define MARIO_GET_VEC(var, type, c_getter) \
    static PyObject * \
    PyMario_get_ ## var(PyMarioStateClass *self, PyObject *arg) { \
        struct MarioState *mario_state = NULL; \
        u32 idx = 0; \
        PyObject *var = 0; \
        idx = PyLong_AsLong(arg); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "mario: during get " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        var = c_getter(mario_state->var[idx]); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "mario: during get " #var ":" ); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        return var; \
    }

/* This messes with refcounts, so make sure this is used EVERYWHERE to set the obj. */
#define MARIO_SET_OBJ(var) \
    PyObject * \
    PyMario_set_ ## var(PyMarioStateClass *self, PyObject *arg) { \
        struct MarioState *mario_state = NULL; \
        struct Object *obj = NULL; \
        if (Py_None != (PyObject *)arg) { \
            obj = python_object_get_native((struct _PyObjectClass *)arg); \
        } \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        /* These refcount adjustments cause corruption in gfx lists. */ \
        /* Py_XDECREF(mario_state->usedObj); */ \
        mario_state->var = obj; \
        /* Py_XINCREF(mario_state->usedObj); */ \
        Py_RETURN_NONE; \
    }

#define MARIO_GET_OBJ(var) \
    static PyObject * \
    PyMario_get_ ## var(PyMarioStateClass *self, PyObject *arg) { \
        struct MarioState *mario_state = NULL; \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        return python_wrap_object(mario_state->var); \
    }

#define MARIO_WRAP_NOARGS(fnc) \
    static PyObject * \
    PyMario_ ## fnc(PyMarioStateClass *self) { \
        struct MarioState *mario_state = NULL; \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        fnc( mario_state ); \
        Py_RETURN_NONE; \
    }

#define MARIO_WRAP_OBJ_ARG(fnc, return_type, return_getter) \
    static PyObject * \
    PyMario_ ## fnc(PyMarioStateClass *self, PyObject *arg) { \
        struct MarioState *mario_state = NULL; \
        struct Object * obj = NULL; \
        return_type retval; \
        mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE); \
        obj = python_object_get_native((struct _PyObjectClass *)arg); \
        retval = fnc( mario_state, obj ); \
        return return_getter( retval ); \
    }

#define MARIO_WRAP_FLAGS(flag_name) \
    static PyObject * \
    PyMario_unset_ ## flag_name(PyObject *self, PyObject *arg) { \
        u32 flag_name; \
        flag_name = PyLong_AsUnsignedLong( arg ); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "during unset flag:"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        gMarioState->flag_name ## s &= ~flag_name; \
        Py_RETURN_NONE; \
    } \
    \
    static PyObject * \
    PyMario_set_ ## flag_name(PyObject *self, PyObject *arg) { \
        u32 flag_name; \
        flag_name = PyLong_AsUnsignedLong( arg ); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "during unset flag:"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        gMarioState->flag_name ## s |= flag_name; \
        Py_RETURN_NONE; \
    }

#ifndef CHECK_PYTHON

static PyObject *
PyMario_facing_downhill(const PyMarioStateClass *self, PyObject *arg) {
    s32 turnYaw;
    s32 res;
    PyObject *pRes;
    struct MarioState *mario_state = NULL;
    
    turnYaw = PyLong_AsLong(arg);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "during facing_downhill:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

    res = mario_facing_downhill(mario_state, turnYaw);
    pRes = PyLong_FromLong(res);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "during facing_downhill:");
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
    
    floorClass = mario_get_floor_class(mario_state);
    
    pRes = PyLong_FromLong( floorClass );
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "during floor_class:");
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
        python_log_error(sLogger, "during setVel:");
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
        python_log_error(sLogger, "mario: during set forwardVel_all:");
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
        python_log_error(sLogger, "mario: during set animID:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);

    mario_state->marioObj->header.gfx.unk38.animID = var;

    Py_RETURN_NONE;
}

static PyObject *
PyMario_push_out_of_object(PyMarioStateClass *self, PyObject *args) {
    struct Object *obj = NULL;
    struct MarioState *mario_state = NULL;
    PyObject *pObjIn = NULL;
    f32 padding = 0.0f;

    PyArg_ParseTuple(args, "Of", &pObjIn, &padding);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "mario: during push out of object:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);
    obj = python_object_get_native((struct _PyObjectClass *)pObjIn);

    push_mario_out_of_object(mario_state, obj, padding);

    Py_RETURN_NONE;
}

static PyObject *
PyMario_get_collided_object(PyMarioStateClass *self, PyObject *arg) {
    struct MarioState *mario_state = NULL;
    u32 interaction_type = 0;
    struct Object *obj_out = NULL;

    mario_state = PYTHON_DECAPSULE_MARIO(self->native_state, Py_RETURN_NONE);
    interaction_type = PyLong_AsUnsignedLong(arg);

    obj_out = mario_get_collided_object(mario_state, interaction_type);

    return python_wrap_object(obj_out);
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
MARIO_SET( capTimer, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET( healCounter, unsigned char, PyLong_AsUnsignedLong );
MARIO_SET( hurtCounter, unsigned char, PyLong_AsUnsignedLong );
MARIO_SET( invincTimer, short, PyLong_AsLong );
MARIO_SET( input, unsigned short, PyLong_AsUnsignedLong );
MARIO_SET_OBJ( interactObj );
MARIO_SET_OBJ( usedObj );
MARIO_SET_OBJ( riddenObj );
MARIO_SET_VEC( vel, float, "f" );
MARIO_SET_VEC( faceAngle, short, "h" );

MARIO_GET( flags, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( action, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( prevAction, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( forwardVel, double, PyFloat_FromDouble );
MARIO_GET( intendedMag, double, PyFloat_FromDouble );
MARIO_GET( intendedYaw, short, PyLong_FromLong );
MARIO_GET( squishTimer, unsigned char, PyLong_FromLong );
MARIO_GET( quicksandDepth, double, PyFloat_FromDouble );
MARIO_GET( actionState, unsigned short, PyLong_FromUnsignedLong );
MARIO_GET( numCoins, short, PyLong_FromLong );
MARIO_GET( numStars, short, PyLong_FromLong );
MARIO_GET( capTimer, unsigned short, PyLong_FromUnsignedLong );
MARIO_GET( healCounter, unsigned char, PyLong_FromUnsignedLong );
MARIO_GET( waterLevel, short, PyLong_FromLong );
MARIO_GET( health, short, PyLong_FromLong );
MARIO_GET( invincTimer, short, PyLong_FromLong );
MARIO_GET( collidedObjInteractTypes, unsigned long, PyLong_FromUnsignedLong );
MARIO_GET( input, unsigned short, PyLong_FromUnsignedLong );
MARIO_GET_OBJ( interactObj );
MARIO_GET_OBJ( usedObj );
MARIO_GET_VEC( vel, float, PyFloat_FromDouble );
MARIO_GET_VEC( pos, float, PyFloat_FromDouble );
MARIO_GET_VEC( faceAngle, short, PyLong_FromLong );

MARIO_WRAP_FLAGS(flag);
MARIO_WRAP_FLAGS(particleFlag);
MARIO_WRAP_FLAGS(collidedObjInteractType);
MARIO_WRAP_NOARGS(mario_stop_riding_and_holding);
MARIO_WRAP_NOARGS(update_mario_sound_and_camera);
MARIO_WRAP_NOARGS(mario_stop_riding_object);
MARIO_WRAP_NOARGS(reset_mario_pitch);
MARIO_WRAP_NOARGS(bully_knock_back_mario);
MARIO_WRAP_NOARGS(mario_drop_held_object);
MARIO_WRAP_NOARGS(check_kick_or_punch_wall);
MARIO_WRAP_OBJ_ARG(mario_obj_angle_to_object, s16, PyLong_FromLong);
MARIO_WRAP_OBJ_ARG(check_read_sign, u32, PyLong_FromUnsignedLong);
MARIO_WRAP_OBJ_ARG(check_npc_talk, u32, PyLong_FromUnsignedLong);

static PyMethodDef PyMarioState_methods[] = {
    {"unset_flag",                  (PyCFunction)PyMario_unset_flag,                METH_O, NULL},
    {"set_flag",                    (PyCFunction)PyMario_set_flag,                  METH_O, NULL},
    {"unset_particle_flag",         (PyCFunction)PyMario_unset_flag,                METH_O, NULL},
    {"set_particle_flag",           (PyCFunction)PyMario_set_flag,                  METH_O, NULL},
    {"unset_collided_obj_interact_type",(PyCFunction)PyMario_unset_collidedObjInteractType, METH_O, NULL},
    {"set_collided_obj_interact_type",  (PyCFunction)PyMario_set_collidedObjInteractType,   METH_O, NULL},
    {"set_action",                  (PyCFunction)PyMario_set_action,                METH_O, NULL},
    {"set_action_state",            (PyCFunction)PyMario_set_actionState,           METH_O, NULL},
    {"set_prev_action",             (PyCFunction)PyMario_set_prevAction,            METH_O, NULL},
    {"set_action_timer",            (PyCFunction)PyMario_set_actionTimer,           METH_O, NULL},
    {"set_action_arg",              (PyCFunction)PyMario_set_actionArg,             METH_O, NULL},
    {"set_forward_vel",             (PyCFunction)PyMario_set_forwardVel,            METH_O, NULL},
    {"set_wall_kick_timer",         (PyCFunction)PyMario_set_wallKickTimer,         METH_O, NULL},
    {"set_peak_height",             (PyCFunction)PyMario_set_peakHeight,            METH_O, NULL},
    {"set_cap_timer",               (PyCFunction)PyMario_set_capTimer,              METH_O, NULL},
    {"set_heal_counter",            (PyCFunction)PyMario_set_healCounter,           METH_O, NULL},
    {"set_hurt_counter",            (PyCFunction)PyMario_set_hurtCounter,           METH_O, NULL},
    {"set_interact_obj",            (PyCFunction)PyMario_set_interactObj,           METH_O, NULL},
    {"set_used_obj",                (PyCFunction)PyMario_set_usedObj,               METH_O, NULL},
    {"set_ridden_obj",              (PyCFunction)PyMario_set_riddenObj,             METH_O, NULL},
    {"set_num_coins",               (PyCFunction)PyMario_set_numCoins,              METH_O, NULL},
    {"set_invinc_timer",            (PyCFunction)PyMario_set_invincTimer,           METH_O, NULL},
    {"set_input",                   (PyCFunction)PyMario_set_input,                 METH_O, NULL},
    {"set_face_angle",              (PyCFunction)PyMario_set_faceAngle,             METH_VARARGS, NULL},

    {"get_pos",                     (PyCFunction)PyMario_get_pos,                   METH_O, NULL},
    {"get_angle_to_object",         (PyCFunction)PyMario_mario_obj_angle_to_object, METH_O, NULL},
    {"get_action",                  (PyCFunction)PyMario_get_action,                METH_NOARGS, NULL},
    {"get_prev_action",             (PyCFunction)PyMario_get_prevAction,            METH_NOARGS, NULL},
    {"get_invinc_timer",            (PyCFunction)PyMario_get_invincTimer,           METH_NOARGS, NULL},
    {"get_forward_vel",             (PyCFunction)PyMario_get_forwardVel,            METH_NOARGS, NULL},
    {"get_intended_mag",            (PyCFunction)PyMario_get_intendedMag,           METH_NOARGS, NULL},
    {"get_intended_yaw",            (PyCFunction)PyMario_get_intendedYaw,           METH_NOARGS, NULL},
    {"get_squish_timer",            (PyCFunction)PyMario_get_squishTimer,           METH_NOARGS, NULL},
    {"get_quicksand_depth",         (PyCFunction)PyMario_get_quicksandDepth,        METH_NOARGS, NULL},
    {"set_vel",                     (PyCFunction)PyMario_set_vel,                   METH_VARARGS, NULL},
    {"get_vel",                     (PyCFunction)PyMario_get_vel,                   METH_O, NULL},
    {"get_num_coins",               (PyCFunction)PyMario_get_numCoins,              METH_NOARGS, NULL},
    {"get_num_stars",               (PyCFunction)PyMario_get_numStars,              METH_NOARGS, NULL},
    {"get_face_angle",              (PyCFunction)PyMario_get_faceAngle,             METH_O, NULL},
    {"get_input",                   (PyCFunction)PyMario_get_input,                 METH_O, NULL},
    {"get_flags",                   (PyCFunction)PyMario_get_flags,                 METH_NOARGS, NULL},
    {"get_action_state",            (PyCFunction)PyMario_get_actionState,           METH_NOARGS, NULL},
    {"get_cap_timer",               (PyCFunction)PyMario_get_capTimer,              METH_NOARGS, NULL},
    {"get_water_level",             (PyCFunction)PyMario_get_waterLevel,            METH_NOARGS, NULL},
    {"get_health",                  (PyCFunction)PyMario_get_health,                METH_NOARGS, NULL},
    {"get_used_obj",                (PyCFunction)PyMario_get_usedObj,               METH_NOARGS, NULL},
    {"get_interact_obj",            (PyCFunction)PyMario_get_interactObj,           METH_NOARGS, NULL},
    {"get_collided_object",         (PyCFunction)PyMario_get_collided_object,       METH_O, NULL},
    {"get_collided_obj_interact_types", (PyCFunction)PyMario_get_collidedObjInteractTypes,      METH_NOARGS, NULL},

    {"stop_riding_and_holding",     (PyCFunction)PyMario_mario_stop_riding_and_holding,     METH_NOARGS, NULL},
    {"stop_riding_object",          (PyCFunction)PyMario_mario_stop_riding_object,          METH_NOARGS, NULL},
    {"update_sound_and_camera",     (PyCFunction)PyMario_update_mario_sound_and_camera,     METH_NOARGS, NULL},
    {"reset_pitch",                 (PyCFunction)PyMario_reset_mario_pitch,                 METH_NOARGS, NULL},
    {"bully_knock_back",            (PyCFunction)PyMario_bully_knock_back_mario,            METH_NOARGS, NULL},
    {"drop_held_object",            (PyCFunction)PyMario_mario_drop_held_object,            METH_NOARGS, NULL},
    {"check_kick_or_punch_wall",    (PyCFunction)PyMario_check_kick_or_punch_wall,          METH_NOARGS, NULL},
    {"check_read_sign",             (PyCFunction)PyMario_check_read_sign,                   METH_O, NULL},
    {"check_npc_talk",              (PyCFunction)PyMario_check_npc_talk,                    METH_O, NULL},

    #ifndef CHECK_PYTHON
    {"set_y_vel_based_on_fspeed",   (PyCFunction)PyMario_set_y_vel_based_on_fspeed, METH_VARARGS, NULL},
    {"push_out_of_object",          (PyCFunction)PyMario_push_out_of_object,        METH_VARARGS, NULL},
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

static PyObject *
PyMario_get_global_timer(PyObject *self) {
    PyObject *pGlobalTimer = NULL;

    pGlobalTimer = PyLong_FromUnsignedLong( gGlobalTimer );
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pGlobalTimer;
}

static PyObject *
PyMario_atan2s(PyObject *self, PyObject *args) {
    f32 a = 0.0f,
        b = 0.0f;
    s16 tan_out = 0;
    PyObject *pTanOut = NULL;

    PyArg_ParseTuple(args, "ff", &a, &b);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "during atan2s:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    tan_out = atan2s(a, b);

    pTanOut = PyLong_FromLong( tan_out );
    if (PyErr_Occurred()) {
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pTanOut;
}

static PyMethodDef PyMarioMethods[] = {
    {"random_float",    (PyCFunction)PyMario_random_float,      METH_NOARGS, NULL},
    {"random_ushort",   (PyCFunction)PyMario_random_ushort,     METH_NOARGS, NULL},
    {"get_global_timer",(PyCFunction)PyMario_random_ushort,     METH_NOARGS, NULL},
    {"atan2s",          (PyCFunction)PyMario_atan2s,            METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef MarioModule = {
    PyModuleDef_HEAD_INIT, "mario", NULL, -1, PyMarioMethods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_mario(void) {
    PyObject *pMario;
    //PyMarioStateClass *pMarioState;

    if (NULL == sLogger) {
        sLogger = python_get_logger("mario");
    }

    if(0 > PyType_Ready( &PyMarioStateType)) {
        python_log_error(sLogger, "type not ready?");
        return NULL;
    }

    pMario = PyModule_Create(&MarioModule);
    if(NULL == pMario) {
        python_log_error(sLogger, "could not allocate mario module");
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

    python_log_info(sLogger, "mario module initialized");

    return pMario;
}

void python_init_mario() {
    PyObject *pObject = NULL;
    PyMarioStateClass *pMarioState = NULL;

    if (NULL == gMarioState->pyState) {
        python_log_debug(sLogger, "setting up mario python state...");
        Py_INCREF(&PyMarioStateType);
        pMarioState = (PyMarioStateClass *)PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
        pMarioState->native_state = PYTHON_ENCAPSULE_MARIO(gMarioState, ;);
        pMarioState->mario_object = NULL;

        gMarioState->pyState = pMarioState;

        assert(PyCapsule_IsValid(pMarioState->native_state, "mario.MarioState._native_state"));
    }

    // Get rid of old Mario wrapper.
    Py_XDECREF(gMarioState->pyState->mario_object);

    pObject = python_wrap_object(gMarioState->marioObj);

    gMarioState->pyState->mario_object = (struct _PyObjectClass *)pObject;

    assert(NULL != gMarioState->pyState->mario_object);

    python_log_debug(sLogger, "new mario dropped");
}

u32 wrap_mario_interaction(
    struct MarioState *m,
    u32 interaction,
    struct Object *obj,
    const char *method
) {
    PyObject *pFunc = NULL,
        *pArgs = NULL,
        *pValue = NULL,
        *pInteraction = NULL,
        *pObj = NULL;
    u32 retval = 0;

    pFunc = PyObject_GetAttrString(gMarioModule, method);
    if (!pFunc || !PyCallable_Check(pFunc)) {
        python_log_error(sLogger, "unable to call interaction: %s", method);
        return 0;
    }

    pArgs = PyTuple_New(3);
        
    /* The tuple will DECREF this for us later. */
    Py_INCREF(m->pyState);
    PyTuple_SetItem(pArgs, 0, (PyObject *)m->pyState);
    
    /* The tuple will DECREF this for us later. */
    pInteraction = PyLong_FromUnsignedLong(interaction);
    PyTuple_SetItem(pArgs, 1, pInteraction);

    /* The tuple will DECREF this for us later. */
    pObj = python_wrap_object(obj);
    PyTuple_SetItem(pArgs, 2, pObj);

    pValue = PyObject_CallObject(pFunc, pArgs);
    
    Py_DECREF(pArgs);
    if(NULL != pValue) {
        retval = PyLong_AsLong(pValue);
        Py_DECREF(pValue);
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return retval;
}

u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method) {
    PyObject *pFunc = NULL,
        *pArgs = NULL,
        *pValue = NULL,
        *pState = NULL,
        *pAction = NULL,
        *pActionArg = NULL;
    u32 retval = 0;

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
