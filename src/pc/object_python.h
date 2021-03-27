
#ifndef OBJECT_PYTHON_H
#define OBJECT_PYTHON_H

#include "types.h"

#include <Python.h>

#define PYCAPSULE_TYPE_OBJECT "objects.Object._native_object"

#define PYTHON_ENCAPSULE_OBJECT(obj, ret_on_fail) \
    PyCapsule_New((void *)obj, PYCAPSULE_TYPE_OBJECT, NULL); \
    if (PyErr_Occurred()) { \
        fprintf(stderr, \
            "during encapsule native object (%s, line %d):\n", \
            __FILE__, __LINE__); \
        PyErr_Print(); \
        ret_on_fail; \
    }

#define PYTHON_DECAPSULE_OBJECT(cap, ret_on_fail) \
    (struct Object *)PyCapsule_GetPointer((PyObject *)cap, \
        PYCAPSULE_TYPE_OBJECT); \
    if (PyErr_Occurred()) { \
        PyErr_Print(); \
        fprintf(stderr, \
            "during decapsule native object (%s, expected " \
                PYCAPSULE_TYPE_OBJECT ") (%s, line %d)\n", \
            PyCapsule_GetName((PyObject *)cap), __FILE__, __LINE__); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, \
                "during attempt to get capsule name (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
        } \
        ret_on_fail; \
    }

#define PYTHON_CALL_OBJECT(args) \
    (struct _PyObjectClass *)PyObject_CallObject( \
        (PyObject *)&PyObjectType, args);

PyObject* PyInit_objects(void);
PyObject* object_python_wrap(struct Object*);

#endif /* OBJECT_PYTHON_H */
