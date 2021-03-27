
#ifndef OBJECT_PYTHON_BEHAVIOR_H
#define OBJECT_PYTHON_BEHAVIOR_H

#include "types.h"

#include <Python.h>

#include "game/object_list_processor.h"

#define PYCAPSULE_TYPE_BEHAVIOR "objects.Behavior._native_behavior"

#define PYTHON_ENCAPSULE_BEHAVIOR(bhv, ret_on_fail) \
    PyCapsule_New((void *)bhv, PYCAPSULE_TYPE_BEHAVIOR, NULL); \
    if (PyErr_Occurred()) { \
        fprintf(stderr, \
            "during encapsule native behavior (%s, line %d):\n", \
            __FILE__, __LINE__); \
        PyErr_Print(); \
        ret_on_fail; \
    }

#define PYTHON_DECAPSULE_BEHAVIOR(cap, ret_on_fail) \
    (BehaviorScript *)PyCapsule_GetPointer((PyObject *)cap, PYCAPSULE_TYPE_BEHAVIOR); \
    if (PyErr_Occurred()) { \
        PyErr_Print(); \
        fprintf(stderr, \
            "during decapsule native behavior (%s, expected " PYCAPSULE_TYPE_BEHAVIOR ") (%s, line %d)\n", \
            PyCapsule_GetName((PyObject *)cap), __FILE__, __LINE__); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, \
                "during attempt to get capsule name (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
        } \
        ret_on_fail; \
    }

#define PYTHON_CALL_BEHAVIOR(args) \
    (struct _PyObjectBehaviorClass *)PyObject_CallObject( \
        (PyObject *)&PyObjectBehaviorType, args);

#define ADD_OBJLIST_CONSTANTS(module) \
    PyModule_AddIntConstant(module, "OBJ_LIST_PLAYER", OBJ_LIST_PLAYER); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_1", OBJ_LIST_UNUSED_1); \
    PyModule_AddIntConstant(module, "OBJ_LIST_DESTRUCTIVE", OBJ_LIST_DESTRUCTIVE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_3", OBJ_LIST_UNUSED_3); \
    PyModule_AddIntConstant(module, "OBJ_LIST_GENACTOR", OBJ_LIST_GENACTOR); \
    PyModule_AddIntConstant(module, "OBJ_LIST_PUSHABLE", OBJ_LIST_PUSHABLE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_LEVEL", OBJ_LIST_LEVEL); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNUSED_7", OBJ_LIST_UNUSED_7); \
    PyModule_AddIntConstant(module, "OBJ_LIST_DEFAULT", OBJ_LIST_DEFAULT); \
    PyModule_AddIntConstant(module, "OBJ_LIST_SURFACE", OBJ_LIST_SURFACE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_POLELIKE", OBJ_LIST_POLELIKE); \
    PyModule_AddIntConstant(module, "OBJ_LIST_SPAWNER", OBJ_LIST_SPAWNER); \
    PyModule_AddIntConstant(module, "OBJ_LIST_UNIMPORTANT", OBJ_LIST_UNIMPORTANT); \
    PyModule_AddIntConstant(module, "NUM_OBJ_LISTS", NUM_OBJ_LISTS);

PyTypeObject PyObjectBehaviorType;
struct _PyObjectBehaviorClass;

BehaviorScript *
PyObjectBehavior_get_native(struct _PyObjectBehaviorClass *self);

#endif /* OBJECT_PYTHON_BEHAVIOR_H */
