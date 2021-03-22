
#ifndef MARIO_PYTHON_H
#define MARIO_PYTHON_H

#include <ultra64.h>

void python_init();
u32 wrap_mario_action(struct MarioState *m, u32 action, u32 arg, const char *method);

#endif /* MARIO_PYTHON_H */
