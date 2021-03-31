
#ifndef DIALOG_PYTHON_H
#define DIALOG_PYTHON_H

#include <Python.h>

#define PYCAPSULE_TYPE_DIALOG "dialog.Dialog._dialog_entry"

PyObject* PyInit_dialog(void);
void dialog_python_render_frame();
void dialog_python_render_dialog();

#endif /* DIALOG_PYTHON_H */
