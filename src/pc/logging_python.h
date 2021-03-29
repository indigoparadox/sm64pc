
#ifndef LOGGING_PYTHON_H
#define LOGGING_PYTHON_H

#include <Python.h>

void python_init_logging();
PyObject *python_get_logger(const char *);
void python_log_debug(PyObject *, const char *, ...);
void python_log_info(PyObject *, const char *, ...);
void python_log_error(PyObject *, const char *, ...);

#endif /* LOGGING_PYTHON_H */
