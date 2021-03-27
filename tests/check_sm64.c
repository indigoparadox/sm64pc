
#include <check.h>

#include <Python.h>

#include "../src/pc/mario_python.h"
#include "../src/pc/mario_python.h"
#include "../src/pc/object_python.h"
#include "../src/pc/level_python.h"
#include "../src/pc/dialog_python.h"

struct MarioState* gMarioState = NULL;

void obj_mark_for_deletion( struct Object* native_object ) {

}

void obj_copy_pos_and_angle(struct Object *dst, struct Object *src) {

}

struct Object* spawn_object_at_origin( struct Object *parent, UNUSED s32 unusedArg, u32 model, const BehaviorScript *behavior ) {
    return NULL;
}

u16 random_u16() {

}

f32 random_float() {

}

void print_text( s16 x, s16 y, const char* str ) {

}

f32 get_additive_y_vel_for_jumps(void) {
    return 0.0f;
}


s32 mario_facing_downhill(struct MarioState *m, s32 turnYaw) {
}

/* This is the hook called by main() to get ALL of python running. */
void python_init() {
    PyObject *pName, *pSysPath, *pCwd, *pFunc;
    char cwd[255];

    Py_SetProgramName( Py_DecodeLocale( "sm64pc", NULL ) );
    if (0 > PyImport_AppendInittab("mario", &PyInit_mario)) {
        fprintf(stderr, "could not add mario to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("objects", &PyInit_objects)) {
        fprintf(stderr, "could not add objects to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("levels", &PyInit_levels)) {
        fprintf(stderr, "could not add levels to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("dialog", &PyInit_dialog)) {
        fprintf(stderr, "could not add levels to inittab\n");
        return;
    }
    Py_Initialize();
    fprintf(stdout, "python initialized\n");

    /* Load the mario module. */
    /* TODO: Load from user dir? */
    pSysPath = PySys_GetObject((char*)"path");
    getcwd(cwd, 255);
    pCwd = PyUnicode_FromString(cwd);
    PyList_Append(pSysPath, pCwd);
    Py_DECREF(pCwd);
    pName = PyUnicode_DecodeFSDefault( "sm64" );
    /* TODO: Error checking of pName left out */

/*
    gMarioModule = PyImport_Import( pName );
    if (PyErr_Occurred()) {
        fprintf( stderr, "during setup:\n" );
        PyErr_Print();
        return;
    }

    pFunc = PyObject_GetAttrString(gMarioModule, "mario_init");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject_CallObject(pFunc, NULL);
        if (PyErr_Occurred()) {
            fprintf( stderr, "during init:\n" );
            PyErr_Print();
        }
    }

    assert( NULL != gMarioModule );
   */
}

int main(int argc, char *argv[]) {
    return 0;
}

