
#ifndef OBJECT_PYTHON_H
#define OBJECT_PYTHON_H

#include "types.h"

#include <Python.h>

#include "python_helpers.h"

#define PYCAPSULE_TYPE_OBJECT "objects.Object._native_object"

#define PYTHON_ENCAPSULE_OBJECT(obj, ret_on_fail) \
    PYTHON_ENCAPSULE(obj, PYCAPSULE_TYPE_OBJECT, sLogger, ret_on_fail)

#define PYTHON_DECAPSULE_OBJECT(cap, ret_on_fail) \
    PYTHON_DECAPSULE(cap, PYCAPSULE_TYPE_OBJECT, struct Object, sLogger, ret_on_fail)

#define PYTHON_CALL_OBJECT(args) \
    (struct _PyObjectClass *)PyObject_CallObject( \
        (PyObject *)&PyObjectType, args);

PyObject* PyInit_objects(void);
PyObject* python_wrap_object(struct Object*);
struct Object *python_object_get_native(struct _PyObjectClass *self);

#endif /* OBJECT_PYTHON_H */
