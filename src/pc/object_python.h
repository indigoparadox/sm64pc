
#ifndef OBJECT_PYTHON_H
#define OBJECT_PYTHON_H

#include "types.h"

#include <Python.h>

typedef struct _PyObjectClass {
    PyObject_HEAD
    struct Object *ptr;
} PyObjectClass;

PyObject* PyInit_objects(void);
PyObject* object_python_wrap(struct Object*);

#endif /* OBJECT_PYTHON_H */
