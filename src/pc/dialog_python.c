
#include "dialog_python.h"

#include "game/ingame_menu.h"

#include "make_const_nonconst.h"
#include "game/print.h"

extern PyObject *gMarioModule;

extern Gfx *gDisplayListHead;

extern u8 gHudSymCoin[];

static const Vtx vertex_text_bg_box[];
extern const Gfx dl_draw_triangle[];
extern const Gfx dl_draw_text_bg_box[];

#define DEFAULT_DIALOG_BOX_ANGLE 90.0f
#define DEFAULT_DIALOG_BOX_SCALE 19.0f

/* Dialog Module */

static PyObject *
PyDialog_print_colorful_text(PyObject *self, PyObject *args) {
    s16 x = 0,
        y = 0;
    const u8 *str;
    int res = 0;
    s32 i;
    s32 j;
    s8 glyphIndex;
    Mtx *mtx;

    res = PyArg_ParseTuple(args, "hhy", &x, &y, &str);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during print:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    print_text(x, y, str);

    Py_RETURN_NONE;
}

static PyMethodDef PyDialog_methods[] = {
    {"print_colorful_text",   (PyCFunction)PyDialog_print_colorful_text,   METH_VARARGS, NULL},
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
