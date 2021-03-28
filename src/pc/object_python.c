
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "game/object_helpers.h"
#include "engine/graph_node.h"

#include "object_python_behavior.h"
#include "object_python_models.h"

extern PyObject *gMarioModule;

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

#define OBJECT_SET( var, addr, type, py_getter ) \
    static PyObject * \
    PyObjects_set_ ## var(PyObjectClass *self, PyObject *args) { \
        struct Object *obj = NULL; \
        type var; \
        var = py_getter(args); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "object: during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE); \
        obj->rawData.asF32[addr] = var; \
        Py_RETURN_NONE; \
    }

#define OBJECT_GET( var, addr, type, c_getter ) \
    static PyObject * \
    PyObjects_get_ ## var(PyObjectClass *self) { \
        struct Object *obj = NULL; \
        PyObject *var; \
        obj = PYTHON_DECAPSULE_OBJECT(self->native_object, Py_RETURN_NONE); \
        var = c_getter(obj->rawData.asF32[addr]); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "object: during set " #var ":\n"); \
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
        fprintf(stderr, "object: during scale:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    self_obj->header.gfx.scale[0] = scale_x;
    self_obj->header.gfx.scale[1] = scale_y;
    self_obj->header.gfx.scale[2] = scale_z;

    obj_apply_scale_to_transform(self_obj);

    Py_RETURN_NONE;
}

PyObject* PyObjects_is_valid(PyObjectClass *self) {
    if (NULL != self &&
    NULL != self->native_object &&
    PyCapsule_IsValid(self->native_object, PYCAPSULE_TYPE_OBJECT)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

OBJECT_SET( oForwardVel,            0x0C, f32, PyFloat_AsDouble );
OBJECT_SET( oVelY,                  0x0A, f32, PyFloat_AsDouble );
OBJECT_GET( oPosY,                  0x07, f32, PyFloat_FromDouble );
OBJECT_SET( oMoveAngleYaw,          0x10, u32, PyLong_AsLong );
OBJECT_SET( oMarioWalkingPitch,     0x10, u32, PyLong_AsLong );
OBJECT_SET( oMarioLongJumpIsSlow,   0x22, s32, PyLong_AsLong );

static PyMethodDef PyObject_methods[] = {
    {"set_forward_vel",             (PyCFunction)PyObjects_set_oForwardVel,             METH_O, NULL},
    {"set_move_angle_yaw",          (PyCFunction)PyObjects_set_oMoveAngleYaw,           METH_O, NULL},
    {"set_vel_y",                   (PyCFunction)PyObjects_set_oVelY,                   METH_O, NULL},
    {"get_pos_y",                   (PyCFunction)PyObjects_get_oPosY,                   METH_NOARGS, NULL},
    {"set_mario_walking_pitch",     (PyCFunction)PyObjects_set_oMarioWalkingPitch,      METH_O, NULL},
    {"set_mario_long_jump_is_slow", (PyCFunction)PyObjects_set_oMarioLongJumpIsSlow,    METH_O, NULL},
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
    /*if (NULL != pParent) {
        Py_INCREF(pParent);
    }*/
    #ifdef PYTHON_DEBUG_VERBOSE
    if(NULL != pParent) {
        fprintf(stdout, "object init parent: 0x%016llx containing 0x%016llx\n", pParent, pParent->native_object);
    }
    #endif /* PYTHON_DEBUG_VERBOSE */
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during spawn:\n");
        PyErr_Print();
        //Py_DECREF(args);
        return 0;
    }
    //Py_DECREF(args);

    if (NULL != pBhv) {
        self->behavior = pBhv;
        Py_INCREF(self->behavior);
        bhv = PyObjectBehavior_get_native(self->behavior);
    }

    if (NULL != pParent && NULL != bhv && 0 <= model) {
        assert(NULL != pParent->native_object);
        parent_obj = PYTHON_DECAPSULE_OBJECT(pParent->native_object, return 0);
        assert(NULL != parent_obj);
        self_obj = spawn_object_at_origin(parent_obj, 0, model, bhv);
        assert(NULL != self_obj);
        self->native_object = PYTHON_ENCAPSULE_OBJECT(self_obj, return 0);
        self_obj->pyObjectState = self;
        /* Decreased again in deallocate_object. */
        Py_INCREF(self);
        //Py_INCREF(self->native_object);
        self->spawned_in_python = 1;
    }

    return 0;
}

void
PyObjects_destroy(PyObjectClass *self) {
    struct Object* native_object = NULL;
    Py_XDECREF(self->behavior);
    if (self->spawned_in_python) {
        native_object = PYTHON_DECAPSULE_OBJECT(self->native_object, ;);
        obj_mark_for_deletion(native_object);
    }
    Py_XDECREF(self->native_object);
    fprintf(stdout, "despawned object\n");
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
    PyObject *pObjects, *pBhvNative, *pBhvArgs;
    struct _PyObjectBehaviorClass *pBhv = NULL;
    //PyMarioStateClass *pMarioState;

    if(0 > PyType_Ready( &PyObjectType)) {
        fprintf( stderr, "type not ready?\n" );
        return NULL;
    }

    if(0 > PyType_Ready( &PyObjectBehaviorType)) {
        fprintf( stderr, "type not ready?\n" );
        return NULL;
    }

    pObjects = PyModule_Create(&ObjectsModule);
    if(NULL == pObjects) {
        fprintf( stderr, "could not allocate objects module\n" );
        return NULL;
    }

    #ifndef CHECK_PYTHON
    OBJECTS_ADD_BEHAVIORS(pObjects);
    OBJECTS_ADD_MODELS(pObjects);
    ADD_OBJLIST_CONSTANTS(pObjects);
    #endif

    Py_INCREF(&PyObjectType);
    if( 0 > PyModule_AddObject(pObjects, "Object", (PyObject *)&PyObjectType)) {
        fprintf(stderr, "unable to add Object to objects module\n");
        Py_DECREF(&PyObjectType);
        Py_DECREF(pObjects);
        return NULL;
    }

    Py_INCREF(&PyObjectBehaviorType);
    if( 0 > PyModule_AddObject(pObjects, "Behavior", (PyObject *)&PyObjectBehaviorType)) {
        fprintf(stderr, "unable to add Behavior to objects module\n");
        Py_DECREF(&PyObjectBehaviorType);
        Py_DECREF(pObjects);
        return NULL;
    }

    fprintf(stdout, "objects module initialized\n");

    return pObjects;
}

/* Native C Interface */

PyObject* object_python_wrap(struct Object *obj) {
    PyObjectClass *pObjectOut;

    assert(NULL != obj);

    if (Py_True == PyObjects_is_valid(obj->pyObjectState)) {
        /* A new object below would have one ref for as long as whatever's
         * calling this uses it, so give it an additional ref if it already
         * exists.
         */
        Py_INCREF(obj->pyObjectState);
        return (PyObject *)obj->pyObjectState;
    }

    pObjectOut = (PyObjectClass *)PyObject_CallObject((PyObject *)&PyObjectType, NULL);
    assert(NULL == pObjectOut->native_object);
    pObjectOut->native_object = PYTHON_ENCAPSULE_OBJECT(obj, Py_RETURN_NONE);
    
    //Py_INCREF(pObjectOut->native_object);
    
    return (PyObject *)pObjectOut;
}

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
        pParent = object_python_wrap( parent );
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
            Py_INCREF(pObjectOut);
            //Py_DECREF(pObjectOut);
        }
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return object_out;
}

struct Object *
python_object_get_native(PyObjectClass *self) {
    struct Object *obj = NULL;
    obj = (struct Object *)PYTHON_DECAPSULE_OBJECT(self->native_object, return NULL);
    return obj;
}
