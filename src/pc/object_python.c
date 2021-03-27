
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "../game/object_helpers.h"

#include "object_python_behavior.h"
#include "object_python_behaviors.h"
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
        obj = PyCapsule_GetPointer(self->native_object, "objects.Object._native_object"); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, "object: during set " #var ":\n"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        obj->rawData.asF32[addr] = var; \
        Py_RETURN_NONE; \
    }

PyObject* PyObjects_copy_pos_and_angle(PyObjectClass *self, PyObjectClass *arg) {
    struct Object *obj_src = NULL,
        *obj_dest = NULL;

    assert(NULL != arg->native_object);
    assert(NULL != self->native_object);

    obj_src = PyCapsule_GetPointer(arg->native_object, "objects.Object._native_object");
    if (PyErr_Occurred()) {
        fprintf(stderr, "during copy_pos_and_angle src:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    obj_dest = PyCapsule_GetPointer(self->native_object, "objects.Object._native_object");
    if (PyErr_Occurred()) {
        fprintf(stderr, "during copy_pos_and_angle dest:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    assert(NULL != obj_src);
    assert(NULL != obj_dest);

    obj_copy_pos_and_angle(obj_dest, obj_src);

    assert(NULL != arg->native_object);
    assert(NULL != self->native_object);

    Py_RETURN_NONE;
}

OBJECT_SET( oForwardVel,            0x0C, f32, PyFloat_AsDouble );
OBJECT_SET( oVelY,                  0x0A, f32, PyFloat_AsDouble );
OBJECT_SET( oMoveAngleYaw,          0x10, u32, PyLong_AsLong );
OBJECT_SET( oMarioWalkingPitch,     0x10, u32, PyLong_AsLong );
OBJECT_SET( oMarioLongJumpIsSlow,   0x22, s32, PyLong_AsLong );

static PyMethodDef PyObject_methods[] = {
    {"set_forward_vel",             (PyCFunction)PyObjects_set_oForwardVel,             METH_O, NULL},
    {"set_move_angle_yaw",          (PyCFunction)PyObjects_set_oMoveAngleYaw,           METH_O, NULL},
    {"set_vel_y",                   (PyCFunction)PyObjects_set_oVelY,                   METH_O, NULL},
    {"set_mario_walking_pitch",     (PyCFunction)PyObjects_set_oMarioWalkingPitch,      METH_O, NULL},
    {"set_mario_long_jump_is_slow", (PyCFunction)PyObjects_set_oMarioLongJumpIsSlow,    METH_O, NULL},
    {"copy_pos_and_angle",          (PyCFunction)PyObjects_copy_pos_and_angle,          METH_O, NULL},
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
        parent_obj = PyCapsule_GetPointer(pParent->native_object, "objects.Object._native_object");
        if (PyErr_Occurred()) {
            fprintf(stderr, "during spawn:\n");
            PyErr_Print();
            return 0;
        }
        assert(NULL != parent_obj);
        self_obj = spawn_object_at_origin(parent_obj, 0, model, bhv);
        assert(NULL != self_obj);
        self->native_object = PYTHON_ENCAPSULE_OBJECT(self_obj, return 0);
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
        native_object = PyCapsule_GetPointer(self->native_object, "objects.Object._native_object");
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
    PyObject *pObjects, *pBhv, *pBhvNative, *pBhvArgs;
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
    OBJECTS_ADD_BEHAVIORS();
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

PyObject* object_python_wrap(struct Object *obj) {
    PyObjectClass *pObjectOut;

    assert(NULL != obj);

    pObjectOut = (PyObjectClass *)PyObject_CallObject((PyObject *)&PyObjectType, NULL);
    assert(NULL == pObjectOut->native_object);
    pObjectOut->native_object = PYTHON_ENCAPSULE_OBJECT(obj, Py_RETURN_NONE);
    //Py_INCREF(pObjectOut);
    //Py_INCREF(pObjectOut->native_object);
    
    return (PyObject *)pObjectOut;
}

/* Native C Interface */

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

        pArgs = PyTuple_New(3);
        
        /* The tuple will DECREF this for us later. */
        pParent = object_python_wrap( parent );
        PyTuple_SetItem(pArgs, 0, pParent);
        
        /* The tuple will DECREF this for us later. */
        pModel = PyLong_FromLong(model);
        PyTuple_SetItem(pArgs, 1, pModel);

        pBhvNative = PyCapsule_New((void *)behavior, "objects.Behavior._native_behavior", NULL);
        if (PyErr_Occurred()) {
            fprintf(stderr, "during wrap:\n");
            PyErr_Print();
            return NULL;
        }
        pBhvArgs = PyTuple_New(1);
        PyTuple_SetItem(pBhvArgs, 0, pBhvNative);
        pBhv = PyObject_CallObject((PyObject *)&PyObjectBehaviorType, pBhvArgs);
        if (PyErr_Occurred()) {
            fprintf(stderr, "during wrap:\n");
            Py_XDECREF(pBhvNative);
            PyErr_Print();
            return NULL;
        }
        Py_DECREF(pBhvArgs);
        
        PyTuple_SetItem(pArgs, 2, pBhv);

        pObjectOut = (PyObjectClass *)PyObject_CallObject(pFunc, pArgs);
        
        Py_XDECREF(pArgs);
        if(NULL != pObjectOut) {
            object_out = PyCapsule_GetPointer( pObjectOut->native_object, "objects.Object._native_object");
            //Py_DECREF(pObjectOut);
        }
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);

    return object_out;
}
