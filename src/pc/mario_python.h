
#ifndef MARIO_PYTHON_H
#define MARIO_PYTHON_H

#include "types.h"

#include <ultra64.h>

#include <Python.h>

struct _PyObjectClass;

void python_init_mario();
PyObject* PyInit_mario(void);
u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method);

#endif /* MARIO_PYTHON_H */
