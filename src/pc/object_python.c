
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "../game/object_helpers.h"

#include "object_python_behaviors.h"
#include "object_python_models.h"

extern PyObject *gMarioModule;

static PyMemberDef PyObject_members[] = {
    {"ptr", T_OBJECT_EX, offsetof(PyObjectClass, ptr), 0, NULL},
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
    struct Object *obj_src = arg->ptr,
        *obj_dest = self->ptr;

    assert(NULL != obj_src);
    assert(NULL != obj_dest);

    obj_copy_pos_and_angle(obj_dest, obj_src);

    Py_RETURN_NONE;
}

static PyMethodDef PyObject_methods[] = {
    //{"spawn_at", PyObject_spawn_at, METH_VARARGS, NULL},
    {"copy_pos_and_angle", PyObject_copy_pos_and_angle, METH_O, NULL},
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

    res = PyArg_ParseTuple(args, "|OlO", &pParent, &model, &pBhv);
    /*if (NULL != pParent) {
        Py_INCREF(pParent);
    }*/
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during spawn:\n");
        PyErr_Print();
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(args);

    if (NULL != bhv) {
        bhv = PyCapsule_GetPointer(pBhv, "objects.behavior");
        if (PyErr_Occurred()) {
            fprintf(stderr, "during spawn:\n");
            PyErr_Print();
            //Py_DECREF(pBhv);
            return NULL;
        }
    }

    //assert(NULL != pParent->ptr);
    //assert(NULL != bhv);

    /*Py_INCREF(&PyObjectType);
    Py_INCREF(self);
    pObjectOut = (PyObjectClass *)PyObject_CallObject((PyObject *)&PyObjectType, NULL);
    if (PyErr_Occurred()) {
        fprintf(stderr, "during spawn:\n");
        PyErr_Print();
        Py_DECREF(pBhv);
        Py_RETURN_NONE;
    }*/

    if (NULL != pParent && NULL != bhv && 0 < model) {
        self->ptr = spawn_object_at_origin(pParent->ptr, 0, model, bhv);
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
        Py_DECREF(&PyObjectType);
        Py_DECREF(pObjects);
        return NULL;
    }

    /* Py_INCREF(&PyMarioStateType);
    pMarioState = PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
    pMarioState->ptr = gMarioState;
    PyModule_AddObject(pMario, "state", pMarioState); 

    gMarioState->pyState = pMarioState;

    #include "mario_python_actions.h"
    #include "mario_python_terrains.h" */

    fprintf(stdout, "objects module initialized\n");

    return pObjects;
}

/*
void object_python_init() {
    if (0 > PyImport_AppendInittab("mario", &PyInit_objects)) {
        fprintf(stderr, "could not add objects to inittab\n");
        return;
    }
    fprintf(stdout, "object module loaded\n");
}
*/

PyObject* object_python_wrap(struct Object *obj) {
    PyObjectClass *pObjectOut;

    assert(NULL != obj);

    pObjectOut = (PyObjectClass *)PyObject_CallObject((PyObject *)&PyObjectType, NULL);
    pObjectOut->ptr = obj;
    if (NULL == pObjectOut->ptr) {
        Py_XDECREF( pObjectOut );
        fprintf(stderr, "unable to spawn object\n" );
        Py_RETURN_NONE;
    }
    Py_INCREF(pObjectOut);
    
    return (PyObject *)pObjectOut;
}
