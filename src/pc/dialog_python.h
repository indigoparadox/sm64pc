
#ifndef DIALOG_PYTHON_H
#define DIALOG_PYTHON_H

#include <Python.h>

#define PYCAPSULE_TYPE_DIALOG "dialog.Dialog._dialog_entry"

#define PYTHON_DIALOG_ADD_CONSTANTS(module) \
    PYTHON_ADD_CONSTANT(module, DIALOG_TYPE_ROTATE); \
    PYTHON_ADD_CONSTANT(module, DIALOG_TYPE_ZOOM);

enum DialogBoxState {
    DIALOG_STATE_OPENING,
    DIALOG_STATE_VERTICAL,
    DIALOG_STATE_HORIZONTAL,
    DIALOG_STATE_CLOSING
};

enum DialogBoxPageState {
    DIALOG_PAGE_STATE_NONE,
    DIALOG_PAGE_STATE_SCROLL,
    DIALOG_PAGE_STATE_END
};

enum DialogBoxType {
    DIALOG_TYPE_ROTATE, // used in NPCs and level messages
    DIALOG_TYPE_ZOOM    // used in signposts and wall signs and etc
};

PyObject* PyInit_dialog(void);
void dialog_python_render_frame();
void dialog_python_render_dialog();

#endif /* DIALOG_PYTHON_H */
