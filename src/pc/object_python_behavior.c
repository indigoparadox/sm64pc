
#include "object_python_behavior.h"

#include <structmember.h>

typedef struct _PyObjectBehaviorClass {
    PyObject_HEAD
    PyObject *native_behavior;
    PyListObject *script;
} PyObjectBehaviorClass;

static PyMemberDef PyObjectBehavior_members[] = {
    {"_native_behavior", T_OBJECT_EX, offsetof(PyObjectBehaviorClass, native_behavior), READONLY, NULL},
    {"script", T_OBJECT_EX, offsetof(PyObjectBehaviorClass, script), 0, NULL},
    {NULL}
};

#if 0
#define _SHIFTL(v, s, w)	\
    ((unsigned int) (((unsigned int)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w)	\
    ((unsigned int)(((unsigned int)(v) >> (s)) & ((0x01 << (w)) - 1)))
#define BC_B(a) _SHIFTL(a, 24, 8)
#define BC_BB(a, b) (_SHIFTL(a, 24, 8) | _SHIFTL(b, 16, 8))
#define BC_BBBB(a, b, c, d) (_SHIFTL(a, 24, 8) | _SHIFTL(b, 16, 8) | _SHIFTL(c, 8, 8) | _SHIFTL(d, 0, 8))
#define BC_BBH(a, b, c) (_SHIFTL(a, 24, 8) | _SHIFTL(b, 16, 8) | _SHIFTL(c, 0, 16))
#define BC_B0H(a, b) (_SHIFTL(a, 24, 8) | _SHIFTL(b, 0, 16))
#define BC_H(a) _SHIFTL(a, 16, 16)
#define BC_HH(a, b) (_SHIFTL(a, 16, 16) | _SHIFTL(b, 0, 16))
#define BC_W(a) ((uintptr_t)(u32)(a))
#define BC_PTR(a) ((uintptr_t)(a))
#endif

static u32 behavior_SHIFTL( u32 v, u32 s, u32 w ) {
    return (
        (unsigned int)(
            (
                (unsigned int)(v) & ((0x01 << (w)) - 1)
            ) << (s)
        )
    );
}

static u32 behavior_BC_B(u32 a) {
    return behavior_SHIFTL(a, 24, 8);
}

static u32 behavior_BC_BB(u32 a, u32 b) {
    return (behavior_SHIFTL(a, 24, 8) | behavior_SHIFTL(b, 16, 8));
}

static PyObject *
PyObject_Behavior_add_to_list(PyObjectBehaviorClass *self, u32 bhv) {
    PyObject *bBegin = NULL;

    bBegin = PyLong_FromUnsignedLong(bhv);
    if (0 > PyList_Append((PyObject *)self->script, bBegin)) {
        fprintf(stderr, "behavior: during append:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    
    Py_RETURN_NONE;
}

static PyObject *
PyObject_Behavior_BEGIN_LOOP(PyObjectBehaviorClass *self, PyObject *arg) {
    return PyObject_Behavior_add_to_list(self, behavior_BC_B(0x08));
}

static PyObject *
PyObject_Behavior_BEGIN(PyObjectBehaviorClass *self, PyObject *arg) {
    enum ObjectList objList;
    
    objList = PyLong_AsLong(arg);
    if (PyErr_Occurred()) {
        fprintf(stderr, "behavior: during BEGIN:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return PyObject_Behavior_add_to_list(self, behavior_BC_BB(0x00, objList));
}

static PyObject *
PyObject_Behavior_END_LOOP(PyObjectBehaviorClass *self, PyObject *arg) {
    return PyObject_Behavior_add_to_list(self, behavior_BC_B(0x09));
}

#if 0

// Delays the behavior script for a certain number of frames.
#define DELAY(num) \
    BC_B0H(0x01, num)

// Jumps to a new behavior command and stores the return address in the object's stack.
#define CALL(addr) \
    BC_B(0x02), \
    BC_PTR(addr)

// Jumps back to the behavior command stored in the object's stack.
#define RETURN() \
    BC_B(0x03)

// Jumps to a new behavior script without saving anything.
#define GOTO(addr) \
    BC_B(0x04), \
    BC_PTR(addr)

// Marks the start of a loop that will repeat a certain number of times.
#define BEGIN_REPEAT(count) \
    BC_B0H(0x05, count)

// Marks the end of a repeating loop.
#define END_REPEAT() \
    BC_B(0x06)

// Also marks the end of a repeating loop, but continues executing commands following the loop on the same frame.
#define END_REPEAT_CONTINUE() \
    BC_B(0x07)

// Exits the behavior script.
// Often used to end behavior scripts that do not contain an infinite loop.
#define BREAK() \
    BC_B(0x0A)

// Exits the behavior script, unused.
#define BREAK_UNUSED() \
    BC_B(0x0B)

// Executes a native game function.
#define CALL_NATIVE(func) \
    BC_B(0x0C), \
    BC_PTR(func)

// Adds a float to the specified field.
#define ADD_FLOAT(field, value) \
    BC_BBH(0x0D, field, value)

// Sets the specified field to a float.
#define SET_FLOAT(field, value) \
    BC_BBH(0x0E, field, value)

// Adds an integer to the specified field.
#define ADD_INT(field, value) \
    BC_BBH(0x0F, field, value)

// Sets the specified field to an integer.
#define SET_INT(field, value) \
    BC_BBH(0x10, field, value)

// Performs a bitwise OR with the specified field and the given integer.
// Usually used to set an object's flags.
#define OR_INT(field, value) \
    BC_BBH(0x11, field, value)

// Performs a bit clear with the specified short. Unused in favor of the 32-bit version.
#define BIT_CLEAR(field, value) \
    BC_BBH(0x12, field, value)

// TODO: this one needs a better name / labelling
// Gets a random short, right shifts it the specified amount and adds min to it, then sets the specified field to that value.
#define SET_INT_RAND_RSHIFT(field, min, rshift) \
    BC_BBH(0x13, field, min), \
    BC_H(rshift)

// Sets the specified field to a random float in the given range.
#define SET_RANDOM_FLOAT(field, min, range) \
    BC_BBH(0x14, field, min), \
    BC_H(range)

// Sets the specified field to a random integer in the given range.
#define SET_RANDOM_INT(field, min, range) \
    BC_BBH(0x15, field, min), \
    BC_H(range)

// Adds a random float in the given range to the specified field.
#define ADD_RANDOM_FLOAT(field, min, range) \
    BC_BBH(0x16, field, min), \
    BC_H(range)

// TODO: better name (unused anyway)
// Gets a random short, right shifts it the specified amount and adds min to it, then adds the value to the specified field. Unused.
#define ADD_INT_RAND_RSHIFT(field, min, rshift) \
    BC_BBH(0x17, field, min), \
    BC_H(rshift)

// No operation. Unused.
#define CMD_NOP_1(field) \
    BC_BB(0x18, field)

// No operation. Unused.
#define CMD_NOP_2(field) \
    BC_BB(0x19, field)

// No operation. Unused.
#define CMD_NOP_3(field) \
    BC_BB(0x1A, field)

// Sets the current model ID of the object.
#define SET_MODEL(modelID) \
    BC_B0H(0x1B, modelID)

// Spawns a child object with the specified model and behavior.
#define SPAWN_CHILD(modelID, behavior) \
    BC_B(0x1C), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Exits the behavior script and despawns the object.
// Often used to end behavior scripts that do not contain an infinite loop.
#define DEACTIVATE() \
    BC_B(0x1D)

// Finds the floor triangle directly under the object and moves the object down to it.
#define DROP_TO_FLOOR() \
    BC_B(0x1E)

// Sets the destination float field to the sum of the values of the given float fields.
#define SUM_FLOAT(fieldDst, fieldSrc1, fieldSrc2) \
    BC_BBBB(0x1F, fieldDst, fieldSrc1, fieldSrc2)

// Sets the destination integer field to the sum of the values of the given integer fields. Unused.
#define SUM_INT(fieldDst, fieldSrc1, fieldSrc2) \
    BC_BBBB(0x20, fieldDst, fieldSrc1, fieldSrc2)

// Billboards the current object, making it always face the camera.
#define BILLBOARD() \
    BC_B(0x21)

#define CYLBOARD() \
    BC_B(0x38)

// Hides the current object.
#define HIDE() \
    BC_B(0x22)

// Sets the size of the object's cylindrical hitbox.
#define SET_HITBOX(radius, height) \
    BC_B(0x23), \
    BC_HH(radius, height)

// No operation. Unused.
#define CMD_NOP_4(field, value) \
    BC_BBH(0x24, field, value)

// Delays the behavior script for the number of frames given by the value of the specified field.
#define DELAY_VAR(field) \
    BC_BB(0x25, field)

// Unused. Marks the start of a loop that will repeat a certain number of times.
// Uses a u8 as the argument, instead of a s16 like the other version does.
#define BEGIN_REPEAT_UNUSED(count) \
    BC_BB(0x26, count)

// Loads the animations for the object. <field> is always set to oAnimations.
#define LOAD_ANIMATIONS(field, anims) \
    BC_BB(0x27, field), \
    BC_PTR(anims)

// Begins animation and sets the object's current animation index to the specified value.
#define ANIMATE(animIndex) \
    BC_BB(0x28, animIndex)

// Spawns a child object with the specified model and behavior, plus a behavior param.
#define SPAWN_CHILD_WITH_PARAM(bhvParam, modelID, behavior) \
    BC_B0H(0x29, bhvParam), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Loads collision data for the object.
#define LOAD_COLLISION_DATA(collisionData) \
    BC_B(0x2A), \
    BC_PTR(collisionData)

// Sets the size of the object's cylindrical hitbox, and applies a downwards offset.
#define SET_HITBOX_WITH_OFFSET(radius, height, downOffset) \
    BC_B(0x2B), \
    BC_HH(radius, height), \
    BC_H(downOffset)

// Spawns a new object with the specified model and behavior.
#define SPAWN_OBJ(modelID, behavior) \
    BC_B(0x2C), \
    BC_W(modelID), \
    BC_PTR(behavior)

// Sets the home position of the object to its current position.
#define SET_HOME() \
    BC_B(0x2D)

// Sets the size of the object's cylindrical hurtbox.
#define SET_HURTBOX(radius, height) \
    BC_B(0x2E), \
    BC_HH(radius, height)

// Sets the object's interaction type.
#define SET_INTERACT_TYPE(type) \
    BC_B(0x2F), \
    BC_W(type)

// Sets various parameters that the object uses for calculating physics.
#define SET_OBJ_PHYSICS(wallHitboxRadius, gravity, bounciness, dragStrength, friction, buoyancy, unused1, unused2) \
    BC_B(0x30), \
    BC_HH(wallHitboxRadius, gravity), \
    BC_HH(bounciness, dragStrength), \
    BC_HH(friction, buoyancy), \
    BC_HH(unused1, unused2)

// Sets the object's interaction subtype. Unused.
#define SET_INTERACT_SUBTYPE(subtype) \
    BC_B(0x31), \
    BC_W(subtype)

// Sets the object's size to the specified percentage.
#define SCALE(unusedField, percent) \
    BC_BBH(0x32, unusedField, percent)

// Performs a bit clear on the object's parent's field with the specified value.
// Used for clearing active particle flags fron Mario's object.
#define PARENT_BIT_CLEAR(field, flags) \
    BC_BB(0x33, field), \
    BC_W(flags)

// Animates an object using texture animation. <field> is always set to oAnimState.
#define ANIMATE_TEXTURE(field, rate) \
    BC_BBH(0x34, field, rate)

// Disables rendering for the object.
#define DISABLE_RENDERING() \
    BC_B(0x35)

// Unused. Sets the specified field to an integer. Wastes 4 bytes of space for no reason at all.
#define SET_INT_UNUSED(field, value) \
    BC_BB(0x36, field), \
    BC_HH(0, value)

// Spawns a water droplet with the given parameters.
#define SPAWN_WATER_DROPLET(dropletParams) \
    BC_B(0x37), \
    BC_PTR(dropletParams)
#endif

static PyObject *
PyObject_Behavior_compile(PyObjectBehaviorClass *self) {
    ssize_t list_sz = 0, i = 0;
    u32 *bhv_compiled = NULL;
    PyObject *pBhvInstruction = NULL;

    assert(NULL == self->native_behavior);

    list_sz = PyList_Size((PyObject *)self->script);
    bhv_compiled = calloc(list_sz, sizeof(u32));
    for(i = 0;list_sz > i;i++) {
        pBhvInstruction = PyList_GetItem((PyObject *)self->script, i);
        bhv_compiled[i] = (u32)PyLong_AsUnsignedLong(pBhvInstruction);
        if (PyErr_Occurred()) {
            fprintf(stderr, "behavior: during compile:\n");
            PyErr_Print();
            Py_RETURN_NONE;
        }
    }

    // TODO: A destructor that frees the ptr.
    self->native_behavior = PYTHON_ENCAPSULE_BEHAVIOR(bhv_compiled, Py_RETURN_NONE);
    fprintf(stderr, "behavior compiled: %ld instructions\n", list_sz);
    assert(PyCapsule_IsValid(self->native_behavior, "objects.Behavior._native_behavior"));

    Py_RETURN_NONE;
}

static PyMethodDef PyObjectBehavior_methods[] = {
    {"compile",                 (PyCFunction)PyObject_Behavior_compile,         METH_NOARGS,    NULL},
    {"bhv_END_LOOP",            (PyCFunction)PyObject_Behavior_END_LOOP,        METH_NOARGS,    NULL},
    {"bhv_BEGIN_LOOP",          (PyCFunction)PyObject_Behavior_BEGIN_LOOP,      METH_NOARGS,    NULL},
    {"bhv_BEGIN",               (PyCFunction)PyObject_Behavior_BEGIN,           METH_O,         NULL},
    {NULL, NULL, 0, NULL}
};

static int
PyObjectBehavior_init(PyObjectBehaviorClass *self, PyObject *args, PyObject *kwds) {
    PyObject *pBhv = NULL;
    int res = 0;

    res = PyArg_ParseTuple(args, "|O", &pBhv);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during behavior init:\n");
        PyErr_Print();
        return 0;
    }
    
    self->script = (PyListObject *)PyList_New(0);
    self->native_behavior = pBhv;
    Py_INCREF(self->script);
    if (NULL != pBhv) {
        Py_INCREF(self->native_behavior);
    }

    assert(
        NULL == pBhv ||
        PyCapsule_IsValid(self->native_behavior, "objects.Behavior._native_behavior"));

    return 0;
}

void
PyObjectBehavior_destroy(PyObjectBehaviorClass *self) {
    Py_XDECREF(self->script);
    // TODO: Free native behavior if compiled in python.
    Py_XDECREF(self->native_behavior);
    fprintf(stdout, "destroyed behavior\n");
}

BehaviorScript *
PyObjectBehavior_get_native(PyObjectBehaviorClass *self) {
    BehaviorScript *bhv = NULL;

    assert(PyCapsule_IsValid(self->native_behavior, "objects.Behavior._native_behavior"));
    bhv = PyCapsule_GetPointer(self->native_behavior, "objects.Behavior._native_behavior");
    if (PyErr_Occurred()) {
        fprintf(stderr, "during get native behavior:\n");
        PyErr_Print();
        return NULL;
    }

    return bhv;
}

PyTypeObject PyObjectBehaviorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "objects.Behavior",
    .tp_basicsize = sizeof(PyObjectBehaviorClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)PyObjectBehavior_init,
    .tp_dealloc = (destructor)PyObjectBehavior_destroy,
    .tp_methods = PyObjectBehavior_methods,
    .tp_members = PyObjectBehavior_members,
};
