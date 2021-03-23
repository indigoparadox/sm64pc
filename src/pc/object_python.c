
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "../game/object_helpers.h"

#include "object_python_behaviors.h"
#include "object_python_models.h"

extern PyObject *gMarioModule;

static PyMemberDef PyObject_members[] = {
    {"_native_object", T_OBJECT_EX, offsetof(PyObjectClass, native_object), 0, NULL},
    {NULL}
};

/*
static PyObject *
PyObject_spawn_at(PyObjectClass *self, PyObject *args) {
    PyObjectClass *pObjectOut;


    return (PyObject *)pObjectOut;
}
*/

PyObject* PyObject_copy_pos_and_angle(PyObjectClass *self, PyObjectClass *arg) {
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

static PyMethodDef PyObject_methods[] = {
    //{"spawn_at", PyObject_spawn_at, METH_VARARGS, NULL},
    {"copy_pos_and_angle", (PyCFunction)PyObject_copy_pos_and_angle, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};

    //obj = spawn_object_at_origin(parent, 0, model, behavior);
    //obj_copy_pos_and_angle(obj, parent);

static int
PyObject_init(PyObjectClass *self, PyObject *args, PyObject *kwds) {
    PyObjectClass *pParent = NULL;
    PyObject *pBhv = NULL;
    s32 model = 0;
    BehaviorScript *bhv = NULL;
    int res = 0;
    struct Object *parent_obj = NULL;
    struct Object *self_obj = NULL;
    u32 test1 = 0;

    res = PyArg_ParseTuple(args, "|lOlO", &test1, &pParent, &model, &pBhv);
    /*if (NULL != pParent) {
        Py_INCREF(pParent);
    }*/
    if(NULL != pParent) {
        fprintf(stdout, "object init parent: 0x%016llx containing 0x%016llx\n", pParent, pParent->native_object);
    }
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during spawn:\n");
        PyErr_Print();
        //Py_DECREF(args);
        return 0;
    }
    //Py_DECREF(args);

    if (NULL != pBhv) {
        bhv = PyCapsule_GetPointer(pBhv, "objects.Behavior");
        if (PyErr_Occurred()) {
            fprintf(stderr, "during spawn:\n");
            PyErr_Print();
            //Py_DECREF(pBhv);
            return 0;
        }
    }

    if (NULL != pParent && NULL != bhv && 0 < model) {
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
        self->native_object = PyCapsule_New(self_obj, "objects.Object._native_object", NULL);
        if (PyErr_Occurred()) {
            fprintf(stderr, "during spawn:\n");
            PyErr_Print();
            return 0;
        }
        assert(NULL != self->native_object);
        Py_INCREF(self->native_object);
    }

    return 0;
}

static PyTypeObject PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "objects.Object",
    .tp_basicsize = sizeof(PyObjectClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)PyObject_init,
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
    PyObject *pObjects, *pBhv;
    //PyMarioStateClass *pMarioState;

    if(0 > PyType_Ready( &PyObjectType)) {
        fprintf( stderr, "type not ready?\n" );
        return NULL;
    }

    pObjects = PyModule_Create(&ObjectsModule);
    if(NULL == pObjects) {
        fprintf( stderr, "could not allocate objects module\n" );
        return NULL;
    }

    OBJECTS_ADD_BEHAVIORS();
    OBJECTS_ADD_MODELS(pObjects);

    Py_INCREF(&PyObjectType);
    if( 0 > PyModule_AddObject(pObjects, "Object", (PyObject *)&PyObjectType)) {
        fprintf(stderr, "unable to add Object to objects module\n");
        Py_DECREF(&PyObjectType);
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
    pObjectOut->native_object = PyCapsule_New(obj, "objects.Object._native_object", NULL);
    if (PyErr_Occurred()) {
        fprintf(stderr, "during spawn:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    assert(NULL != pObjectOut->native_object);
    Py_INCREF(pObjectOut);
    Py_INCREF(pObjectOut->native_object);
    
    return (PyObject *)pObjectOut;
}
