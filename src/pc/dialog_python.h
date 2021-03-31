
#ifndef DIALOG_PYTHON_H
#define DIALOG_PYTHON_H

#include <Python.h>

PyObject* PyInit_dialog(void);
void dialog_python_render_frame();
void dialog_python_render_dialog();

#endif /* DIALOG_PYTHON_H */
