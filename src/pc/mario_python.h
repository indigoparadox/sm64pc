
#ifndef MARIO_PYTHON_H
#define MARIO_PYTHON_H

#include "types.h"

#include <ultra64.h>

#include <Python.h>

#include "python_helpers.h"

#define PYCAPSULE_TYPE_MARIO "mario.MarioState._native_state"

#define PYTHON_ENCAPSULE_MARIO(obj, ret_on_fail) \
    PYTHON_ENCAPSULE(obj, PYCAPSULE_TYPE_MARIO, ret_on_fail)
    
#define PYTHON_DECAPSULE_MARIO(cap, ret_on_fail) \
    PYTHON_DECAPSULE(cap, PYCAPSULE_TYPE_MARIO, struct MarioState, ret_on_fail)

struct _PyObjectClass;

void python_init_mario();
PyObject* PyInit_mario(void);
u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method);

#endif /* MARIO_PYTHON_H */
