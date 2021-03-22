
#include "object_python.h"

#include <Python.h>
#include <structmember.h>

#include "object_python_behaviors.h"

extern PyObject *gMarioModule;

typedef struct {
    PyObject_HEAD
    int *ptr;
} PyObjectClass;

static PyMemberDef PyObject_members[] = {
    {"ptr", T_OBJECT_EX, offsetof(PyObjectClass, ptr), 0, NULL},
    {NULL}
};

static PyMethodDef PyObject_methods[] = {
    {NULL, NULL, 0, NULL}
};

    //obj = spawn_object_at_origin(parent, 0, model, behavior);
    //obj_copy_pos_and_angle(obj, parent);

static int
PyObject_init(PyObjectClass *self, PyObject *args, PyObject *kwds) {


    //self->ptr = spawn_object_at_origin(parent, 0, model, behavior);

    return 0;
}

static PyTypeObject PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "objects.Object",
    .tp_basicsize = sizeof(PyObjectClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = PyObject_init,
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

static PyObject* PyInit_objects(void) {
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

    /*Py_INCREF(&PyMarioStateType);
    if( 0 > PyModule_AddObject(pMario, "MarioState", (PyObject *)&PyMarioStateType)) {
        Py_DECREF(&PyMarioStateType);
        Py_DECREF(pMario);
        return NULL;
    }*/

    /* Py_INCREF(&PyMarioStateType);
    pMarioState = PyObject_CallObject((PyObject *)&PyMarioStateType, NULL);
    pMarioState->ptr = gMarioState;
    PyModule_AddObject(pMario, "state", pMarioState); 

    gMarioState->pyState = pMarioState;

    #include "mario_python_actions.h"
    #include "mario_python_terrains.h" */

    return pObjects;
}

void object_python_init() {
    PyImport_AppendInittab("objects", &PyInit_objects);
}
