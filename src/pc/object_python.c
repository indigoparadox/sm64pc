
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "game/object_helpers.h"
#include "game/mario.h"
#include "engine/graph_node.h"

#include "object_python_behavior.h"
#include "object_python_models.h"

#include "logging_python.h"

extern PyObject *gMarioModule;

static PyObject *sLogger = NULL;
PyObject *gLoggerBehavior = NULL;

u32 get_mario_cap_flag(struct Object *);
void obj_apply_scale_to_transform(struct Object *);

typedef struct _PyObjectClass {
    PyObject_HEAD 
    PyObject *native_object;
    struct _PyObjectBehaviorClass *behavior;
    char spawned_in_python;
} PyObjectClass;

static PyMemberDef PyObject_members[] = {
    {"_native_object", T_OBJECT_EX, offsetof(PyObjectClass, native_object), READONLY, NULL},
    {"behavior", T_OBJECT_EX, offsetof(PyObjectClass, behavior), READONLY, NULL},
    {"_spawned_in_python", T_BOOL, offsetof(PyObjectClass, spawned_in_python), READONLY, NULL},
    {NULL}
};

#define OBJECT_SET( var, addr, type, py_getter, as_union ) \
    static PyObject * \
    PyObjects_set_ ## var(PyObjectClass *self, PyObject *args) { \
        struct Object *obj = NULL; \
        type var; \
        var = py_getter(args); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "object: during set " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE); \
        obj->rawData.as_union[addr] = var; \
        Py_RETURN_NONE; \
    }

#define OBJECT_GET( var, addr, type, c_getter, as_union ) \
    static PyObject * \
    PyObjects_get_ ## var(PyObjectClass *self) { \
        struct Object *obj = NULL; \
        PyObject *var; \
        obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE); \
        var = c_getter(obj->rawData.as_union[addr]); \
        if (PyErr_Occurred()) { \
            python_log_error(sLogger, "object: during set " #var ":"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        return var; \
    }

PyObject* PyObjects_copy_pos_and_angle(PyObjectClass *self, PyObjectClass *arg) {
    struct Object *obj_src = NULL,
        *obj_dest = NULL;
    obj_src = PYTHON_DECAPSULE_OBJECT(arg->native_object, Py_RETURN_NONE);
    obj_dest = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);
    obj_copy_pos_and_angle(obj_dest, obj_src);
    Py_RETURN_NONE;
}

PyObject* PyObjects_init_animation(PyObjectClass *self, PyObject *arg) {
    struct Object *self_obj = NULL;
    struct Animation **anims = NULL;
    s32 anim_index = 0;

    if (NULL == self_obj) {
        Py_RETURN_NONE;
    }

    self_obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);

    anims = (struct Animation **)self_obj->ptrData.asAnims[0x26];

    if (NULL == self_obj) {
        Py_RETURN_NONE;
    }

    anim_index = PyLong_AsLong(arg);

    geo_obj_init_animation(&self_obj->header.gfx, &anims[anim_index]);

    Py_RETURN_NONE;
}

PyObject* PyObjects_scale(PyObjectClass *self, PyObject *args) {
    struct Object *self_obj = NULL;
    f32 scale_x = 0.0f,
        scale_y = 0.0f,
        scale_z = 0.0f;

    self_obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);

    if (NULL == self_obj) {
        Py_RETURN_NONE;
    }

    PyArg_ParseTuple(args, "fff", &scale_x, &scale_y, &scale_z);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during scale:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    self_obj->header.gfx.scale[0] = scale_x;
    self_obj->header.gfx.scale[1] = scale_y;
    self_obj->header.gfx.scale[2] = scale_z;

    obj_apply_scale_to_transform(self_obj);

    Py_RETURN_NONE;
}

PyObject* PyObjects_set_angle(PyObjectClass *self, PyObject *args) {
    struct Object *self_obj = NULL;
    s16 pitch = 0.0f,
        yaw = 0.0f,
        roll = 0.0f;

    self_obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);

    if (NULL == self_obj) {
        Py_RETURN_NONE;
    }

    PyArg_ParseTuple(args, "hhh", &pitch, &yaw, &roll);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during set angle:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    obj_set_angle(self_obj, pitch, yaw, roll);

    Py_RETURN_NONE;
}

static PyObject *
PyObjects_get_hitbox_radius(PyObjectClass *self) {
    struct Object *obj = NULL;
    PyObject *var;
    obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);
    var = PyFloat_FromDouble(obj->hitboxRadius);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during get hitbox radius:");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    return var;
}

static PyObject *
PyObjects_get_hitbox_height(PyObjectClass *self) {
    struct Object *obj = NULL;
    PyObject *var;
    obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);
    var = PyFloat_FromDouble(obj->hitboxHeight);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during get hitbox height:");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    return var;
}

static PyObject *
PyObjects_get_collided_obj_interact_types(PyObjectClass *self) {
    struct Object *obj = NULL;
    PyObject *var;
    obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);
    var = PyLong_FromUnsignedLong(obj->collidedObjInteractTypes);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during get collided obj interact types:");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    return var;
}

static PyObject *
PyObjects_get_mario_cap_flag(PyObjectClass *self) {
    struct Object *obj = NULL;
    PyObject *var;
    u32 cap_flag = 0;

    obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE);
    cap_flag = get_mario_cap_flag( obj );

    var = PyLong_FromUnsignedLong(cap_flag);
    if (PyErr_Occurred()) {
        python_log_error(sLogger, "object: during get cap flag:");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    return var;
}

PyObject* PyObjects_is_valid(PyObjectClass *self) {
    if (NULL != self &&
    NULL != self->native_object &&
    PyCapsule_IsValid(self->native_object, PYCAPSULE_TYPE_OBJECT)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

OBJECT_SET( oForwardVel,            0x0C, f32, PyFloat_AsDouble, asF32 );
OBJECT_SET( oVelY,                  0x0A, f32, PyFloat_AsDouble, asF32 );
OBJECT_SET( oMoveAngleYaw,          0x10, u32, PyLong_AsUnsignedLong, asU32 );
OBJECT_SET( oMarioWalkingPitch,     0x10, u32, PyLong_AsUnsignedLong, asU32 );
OBJECT_SET( oMarioLongJumpIsSlow,   0x22, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oDamageOrCoinValue,     0x3e, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oInteractStatus,        0x2b, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oInteractionSubtype,    0x42, u32, PyLong_AsUnsignedLong, asU32 );
OBJECT_SET( oBehParams,             0x40, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioTornadoYawVel,    0x21, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioTornadoPosY,      0x22, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioBurnTimer,        0x22, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioPoleUnk108,       0x20, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioPoleYawVel,       0x21, s32, PyLong_AsLong, asS32 );
OBJECT_SET( oMarioPolePos,          0x22, f32, PyFloat_AsDouble, asF32 );
OBJECT_SET( oMarioWhirlpoolPosY,    0x22, f32, PyFloat_AsDouble, asF32 );

OBJECT_GET( oPosX,                  0x06, f32, PyFloat_FromDouble, asF32 );
OBJECT_GET( oPosY,                  0x07, f32, PyFloat_FromDouble, asF32 );
OBJECT_GET( oPosZ,                  0x08, f32, PyFloat_FromDouble, asF32 );
OBJECT_GET( oDamageOrCoinValue,     0x3e, s32, PyLong_FromLong, asS32 );
OBJECT_GET( oInteractionSubtype,    0x42, u32, PyLong_FromUnsignedLong, asU32 );
OBJECT_GET( oInteractStatus,        0x2b, s32, PyLong_FromLong, asS32 );
OBJECT_GET( oBehParams,             0x40, s32, PyLong_FromLong, asS32 );
OBJECT_GET( oMoveAngleYaw,          0x10, u32, PyLong_FromUnsignedLong, asU32 );
OBJECT_GET( oMarioBurnTimer,        0x22, s32, PyLong_FromLong, asS32 );

static PyMethodDef PyObject_methods[] = {
    {"set_forward_vel",             (PyCFunction)PyObjects_set_oForwardVel,             METH_O, NULL},
    {"set_move_angle_yaw",          (PyCFunction)PyObjects_set_oMoveAngleYaw,           METH_O, NULL},
    {"set_vel_y",                   (PyCFunction)PyObjects_set_oVelY,                   METH_O, NULL},
    {"set_mario_walking_pitch",     (PyCFunction)PyObjects_set_oMarioWalkingPitch,      METH_O, NULL},
    {"set_mario_long_jump_is_slow", (PyCFunction)PyObjects_set_oMarioLongJumpIsSlow,    METH_O, NULL},
    {"set_damage_or_coin_value",    (PyCFunction)PyObjects_set_oDamageOrCoinValue,      METH_O, NULL},
    {"set_interact_status",         (PyCFunction)PyObjects_set_oInteractStatus,         METH_O, NULL},
    {"set_interaction_subtype",     (PyCFunction)PyObjects_set_oInteractionSubtype,     METH_O, NULL},
    {"set_beh_params",              (PyCFunction)PyObjects_set_oBehParams,              METH_O, NULL},
    {"set_angle",                   (PyCFunction)PyObjects_set_angle,                   METH_VARARGS, NULL},
    {"set_mario_tornado_yaw_vel",   (PyCFunction)PyObjects_set_oMarioTornadoYawVel,     METH_O, NULL},
    {"set_mario_tornato_pos_y",     (PyCFunction)PyObjects_set_oMarioTornadoPosY,       METH_O, NULL},
    {"set_mario_burn_timer",        (PyCFunction)PyObjects_set_oMarioBurnTimer,         METH_O, NULL},
    {"set_mario_pole_unk108",       (PyCFunction)PyObjects_set_oMarioPoleUnk108,        METH_O, NULL},
    {"set_mario_pole_yaw_vel",      (PyCFunction)PyObjects_set_oMarioPoleYawVel,        METH_O, NULL},
    {"set_mario_pole_pos",          (PyCFunction)PyObjects_set_oMarioPolePos,           METH_O, NULL},
    {"set_mario_whirlpool_pos_y",   (PyCFunction)PyObjects_set_oMarioWhirlpoolPosY,     METH_O, NULL},

    {"get_pos_x",                   (PyCFunction)PyObjects_get_oPosX,                   METH_NOARGS, NULL},
    {"get_pos_y",                   (PyCFunction)PyObjects_get_oPosY,                   METH_NOARGS, NULL},
    {"get_pos_z",                   (PyCFunction)PyObjects_get_oPosZ,                   METH_NOARGS, NULL},
    {"get_damage_or_coin_value",    (PyCFunction)PyObjects_get_oDamageOrCoinValue,      METH_NOARGS, NULL},
    {"get_interaction_subtype",     (PyCFunction)PyObjects_get_oInteractionSubtype,     METH_NOARGS, NULL},
    {"get_interact_status",         (PyCFunction)PyObjects_get_oInteractStatus,         METH_NOARGS, NULL},
    {"get_beh_params",              (PyCFunction)PyObjects_get_oBehParams,              METH_NOARGS, NULL},
    {"get_move_angle_yaw",          (PyCFunction)PyObjects_get_oMoveAngleYaw,           METH_NOARGS, NULL},
    {"get_hitbox_radius",           (PyCFunction)PyObjects_get_hitbox_radius,           METH_NOARGS, NULL},
    {"get_hitbox_height",           (PyCFunction)PyObjects_get_hitbox_height,           METH_NOARGS, NULL},
    {"get_mario_burn_timer",        (PyCFunction)PyObjects_get_oMarioBurnTimer,         METH_NOARGS, NULL},
    {"get_mario_cap_flag",          (PyCFunction)PyObjects_get_mario_cap_flag,          METH_NOARGS, NULL},
    {"get_collided_obj_interact_types", (PyCFunction)PyObjects_get_collided_obj_interact_types, METH_NOARGS, NULL},

    {"init_animation",              (PyCFunction)PyObjects_init_animation,              METH_O, NULL},
    {"scale",                       (PyCFunction)PyObjects_scale,                       METH_VARARGS, NULL},
    {"copy_pos_and_angle",          (PyCFunction)PyObjects_copy_pos_and_angle,          METH_O, NULL},
    {"is_valid",                    (PyCFunction)PyObjects_is_valid,                    METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static int
PyObjects_init(PyObjectClass *self, PyObject *args, PyObject *kwds) {
    PyObjectClass *pParent = NULL;
    struct _PyObjectBehaviorClass *pBhv = NULL;
    s32 model = 0;
    BehaviorScript *bhv = NULL;
    int res = 0;
    struct Object *parent_obj = NULL;
    struct Object *self_obj = NULL;

    res = PyArg_ParseTuple(args, "|OlO", &pParent, &model, &pBhv);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during spawn:");
        PyErr_Print();
        return 0;
    }

    if (NULL != pBhv) {
        self->behavior = pBhv;
        bhv = PyObjectBehavior_get_native(self->behavior);
    } else {
        self->behavior = (struct _PyObjectBehaviorClass *)Py_None;
    }
    Py_INCREF(self->behavior);

    self->spawned_in_python = 0;
    if (NULL != pParent && NULL != bhv && 0 <= model) {
        assert(NULL != pParent->native_object);
        parent_obj = PYTHON_DECAPSULE_OBJECT(pParent->native_object, return 0);
        assert(NULL != parent_obj);
        self_obj = spawn_object_at_origin(parent_obj, 0, model, bhv);
        assert(NULL != self_obj);
        self->native_object = PYTHON_ENCAPSULE_OBJECT(self_obj, return 0);
        self_obj->pyObjectState = self;
        self->spawned_in_python = 1;
    }

    return 0;
}

void
PyObjects_destroy(PyObjectClass *self) {
    struct Object* native_object = NULL;
    Py_XDECREF(self->behavior);
    native_object = PYTHON_DECAPSULE_OBJECT(self->native_object, ;);
    native_object->pyObjectState = NULL;
    Py_DECREF(self->native_object);
    #ifdef PYTHON_MEM_DEBUG
    python_log_debug(sLogger, "despawned python object for obj %llx", native_object);
    #endif /* PYTHON_MEM_DEBUG */
}

static PyTypeObject PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "objects.Object",
    .tp_basicsize = sizeof(PyObjectClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)PyObjects_init,
    .tp_dealloc = (destructor)PyObjects_destroy,
    .tp_methods = PyObject_methods,
    .tp_members = PyObject_members,
};

/* Objects Module */

static PyMethodDef PyObjects_methods[] = {
    {NULL, NULL, 0, NULL}
};

static PyModuleDef ObjectsModule = {
    PyModuleDef_HEAD_INIT, "objects", NULL, -1, PyObjects_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_objects(void) {
    PyObject *pObjects = NULL,
        *pBhvNative = NULL,
        *pBhvArgs = NULL;
    struct _PyObjectBehaviorClass *pBhv = NULL;
    //PyMarioStateClass *pMarioState;

    if (NULL == sLogger) {
        sLogger = python_get_logger("objects");
    }
    if (NULL == gLoggerBehavior) {
        gLoggerBehavior = python_get_logger("objects.behavior");
    }

    if(0 > PyType_Ready( &PyObjectType)) {
        python_log_error(sLogger, "type not ready?" );
        return NULL;
    }

    if(0 > PyType_Ready( &PyObjectBehaviorType)) {
        python_log_error(sLogger, "type not ready?" );
        return NULL;
    }

    pObjects = PyModule_Create(&ObjectsModule);
    if(NULL == pObjects) {
        python_log_error(sLogger, "could not allocate objects module" );
        return NULL;
    }

    #ifndef CHECK_PYTHON
    OBJECTS_ADD_BEHAVIORS(pObjects);
    OBJECTS_ADD_MODELS(pObjects);
    ADD_OBJLIST_CONSTANTS(pObjects);
    #endif

    Py_INCREF(&PyObjectType);
    if( 0 > PyModule_AddObject(pObjects, "Object", (PyObject *)&PyObjectType)) {
        python_log_error(sLogger, "unable to add Object to objects module");
        Py_DECREF(&PyObjectType);
        Py_DECREF(pObjects);
        return NULL;
    }

    Py_INCREF(&PyObjectBehaviorType);
    if( 0 > PyModule_AddObject(pObjects, "Behavior", (PyObject *)&PyObjectBehaviorType)) {
        python_log_error(sLogger, "unable to add Behavior to objects module");
        Py_DECREF(&PyObjectBehaviorType);
        Py_DECREF(pObjects);
        return NULL;
    }

    python_log_info(sLogger, "objects module initialized");

    return pObjects;
}

/* Native C Interface */

PyObject* python_wrap_object(struct Object *obj) {
    PyObjectClass *pObjectOut = NULL;
    PyObject *pBehaviorCapsule = NULL,
        *pBehaviorArgs = NULL;

    assert(NULL != obj);

    if (Py_True == PyObjects_is_valid(obj->pyObjectState)) {
        /* A new object below would have one ref for as long as whatever's
         * calling this uses it, so give it an additional ref if it already
         * exists.
         */
        Py_INCREF(obj->pyObjectState);
        return (PyObject *)obj->pyObjectState;
    }

    #ifdef PYTHON_MEM_DEBUG
    python_log_debug(sLogger, "wrapping native obj %llx in python object", obj);
    #endif /* PYTHON_MEM_DEBUG */

    pObjectOut = (PyObjectClass *)PyObject_CallObject((PyObject *)&PyObjectType, NULL);
    assert(NULL == pObjectOut->native_object);
    pObjectOut->native_object = PYTHON_ENCAPSULE_OBJECT(obj, Py_RETURN_NONE);
    assert(NULL != pObjectOut->native_object);

    Py_DECREF(pObjectOut->behavior);
    pBehaviorArgs = PyTuple_New(1);
    pBehaviorCapsule = PYTHON_ENCAPSULE(obj->behavior, "objects.Behavior._native_behavior", ;);
    PyTuple_SetItem(pBehaviorArgs, 0, pBehaviorCapsule);
    pObjectOut->behavior = (struct _PyObjectBehaviorClass *)PyObject_CallObject((PyObject *)&PyObjectBehaviorType, pBehaviorArgs);
    Py_DECREF(pBehaviorArgs);

    return (PyObject *)pObjectOut;
}

#if 0

struct Object *
wrap_spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior) {
    PyObjectClass *pObjectOut = NULL;
    PyObject *pFunc = NULL,
        *pArgs = NULL,
        *pParent = NULL,
        *pModel = NULL,
        *pBhvNative = NULL,
        *pBhv = NULL,
        *pBhvArgs = NULL;
    struct Object *object_out = NULL;

    pFunc = PyObject_GetAttrString(gMarioModule, "spawn_object");
    if (pFunc && PyCallable_Check(pFunc)) {

        /* pBhvNative Ref: 1 */
        pBhvNative = PYTHON_ENCAPSULE_BEHAVIOR(behavior, return NULL);
        
        pBhvArgs = PyTuple_New(1);
        PyTuple_SetItem(pBhvArgs, 0, pBhvNative);

        /* pBhvNative Ref: 2, pBhv Ref: 1 */
        pBhv = PyObject_CallObject((PyObject *)&PyObjectBehaviorType, pBhvArgs);
        if (PyErr_Occurred()) {
            fprintf(stderr, "during wrap:\n");
            Py_XDECREF(pBhvNative);
            PyErr_Print();
            return NULL;
        }
        
        /* pBhvNative Ref: 1 */
        Py_DECREF(pBhvArgs);
        
        pArgs = PyTuple_New(3);
        
        /* The tuple will DECREF this for us later. */
        pParent = python_wrap_object( parent );
        PyTuple_SetItem(pArgs, 0, pParent);
        
        /* The tuple will DECREF this for us later. */
        pModel = PyLong_FromLong(model);
        PyTuple_SetItem(pArgs, 1, pModel);
    
        PyTuple_SetItem(pArgs, 2, pBhv);

        pObjectOut = (PyObjectClass *)PyObject_CallObject(pFunc, pArgs);
        
        /* pBhv Ref: 0 */
        Py_XDECREF(pArgs);
        if(NULL != pObjectOut) {
            object_out = PYTHON_DECAPSULE_OBJECT(pObjectOut->native_object, ;);
            object_out->pyObjectState = pObjectOut;
            /* Decreased again in deallocate_object. */
            //Py_INCREF(pObjectOut);
            //Py_DECREF(pObjectOut);
        }
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return object_out;
}

#endif

struct Object *
python_object_get_native(PyObjectClass *self) {
    struct Object *obj = NULL;
    obj = (struct Object *)PYTHON_DECAPSULE_OBJECT(self->native_object, return NULL);
    return obj;
}
