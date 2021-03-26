
#include "dialog_python.h"

#include "game/ingame_menu.h"

#include "make_const_nonconst.h"

extern PyObject *gMarioModule;

extern Gfx *gDisplayListHead;

static const Vtx vertex_text_bg_box[];
extern const Gfx dl_draw_triangle[];
extern const Gfx dl_draw_text_bg_box[];

#define DEFAULT_DIALOG_BOX_ANGLE 90.0f
#define DEFAULT_DIALOG_BOX_SCALE 19.0f

/* Dialog Module */

static PyObject *
PyDialog_print_generic_string(PyObject *self, PyObject *args) {
    s16 x = 0,
        y = 0;
    const u8 *str;
    int res = 0;

    res = PyArg_ParseTuple(args, "hhy", &x, &y, &str);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during print:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    /*create_dl_translation_matrix(MENU_MTX_PUSH, x - 78, y - 32, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.2f, 0.8f, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 105);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);*/

    /* create_dl_translation_matrix(MENU_MTX_PUSH, x + 6, y - 28, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW); */

    /* create_dl_translation_matrix(MENU_MTX_PUSH, x - 9, y - 101, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 270.0f, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW); */

    print_generic_string(x, y, str);

    Py_RETURN_NONE;
}

static PyMethodDef PyDialog_methods[] = {
    {"print_generic_string",   (PyCFunction)PyDialog_print_generic_string,   METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PyDialogModule = {
    PyModuleDef_HEAD_INIT, "dialog", NULL, -1, PyDialog_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_dialog(void) {
    PyObject *pDialog;

    pDialog = PyModule_Create(&PyDialogModule);
    if(NULL == pDialog) {
        fprintf( stderr, "could not allocate dialog module\n" );
        return NULL;
    }

    fprintf(stdout, "dialog module initialized\n");

    return pDialog;
}

void dialog_python_render_frame() {
    PyObject *pFunc = NULL;

    pFunc = PyObject_GetAttrString(gMarioModule, "dialog_render_frame");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject_CallObject(pFunc, NULL);
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);
}
