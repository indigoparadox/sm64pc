
#include "logging_python.h"

#include <stdio.h>
#include <stdarg.h>

PyObject *sLogging = NULL;

void python_init_logging() {
    PyObject *pBasicConfig = NULL,
        *pArgs = NULL,
        *pKWArgs = NULL,
        *pLevelDebug = NULL;
    
    /* Setup convenient logging. */
    sLogging = PyImport_ImportModule("logging");
    
    pBasicConfig = PyObject_GetAttrString(sLogging, "basicConfig");
    if (!pBasicConfig || !PyCallable_Check(pBasicConfig)) {
        fprintf(stderr, "could not setup logging\n");
        return;
    }

    pLevelDebug = PyObject_GetAttrString(sLogging, "DEBUG");
    if (!pLevelDebug) {
        fprintf(stderr, "could not find DEBUG loglevel\n");
        return;
    }

    pArgs = PyTuple_New(0);
    pKWArgs = Py_BuildValue("{s:O}", "level", pLevelDebug);
    PyObject_Call(pBasicConfig, pArgs, pKWArgs);
    if (PyErr_Occurred()) {
        fprintf( stderr, "during logging config:\n" );
        PyErr_Print();
        return;
    }
    Py_DECREF(pArgs);
    Py_DECREF(pKWArgs);
    Py_DECREF(pBasicConfig);
}

PyObject *python_get_logger(const char *logger_name) {
    PyObject *pGetLogger = NULL,
        *pLoggerName = NULL,
        *pLogger = NULL,
        *pArgs = NULL;

    pGetLogger = PyObject_GetAttrString(sLogging, "getLogger");
    if (!pGetLogger || !PyCallable_Check(pGetLogger)) {
        fprintf(stderr, "could not get getLogger\n");
        Py_RETURN_NONE;
    }

    pLoggerName = PyUnicode_FromString(logger_name);
    pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, pLoggerName);

    pLogger = PyObject_Call(pGetLogger, pArgs, NULL);
    if (!pGetLogger || !PyCallable_Check(pGetLogger)) {
        fprintf(stderr, "could not get logger\n");
    }

    Py_DECREF(pArgs);
    Py_DECREF(pGetLogger);
    
    return pLogger;
}

static void
python_log_vargs(PyObject *pLogger, const char *method, const char *fmt, va_list vargs) {
    PyObject *pLog = NULL,
        *pArgs = NULL,
        *pMsg = NULL;
    char log_buffer[1024];

    pLog = PyObject_GetAttrString(pLogger, "debug");
    if (!pLog || !PyCallable_Check(pLog)) {
        fprintf(stderr, "could not get log method\n");
        return;
    }

    vsnprintf(log_buffer, 1024, fmt, vargs);
    pMsg = PyUnicode_FromString(log_buffer);
    pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, pMsg);

    PyObject_Call(pLog, pArgs, NULL);
    Py_DECREF(pArgs);
    Py_DECREF(pLog);
}

void python_log_debug(PyObject *pLogger, const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);
    python_log_vargs(pLogger, "debug", fmt, vargs);
    va_end(vargs);
}

void python_log_info(PyObject *pLogger, const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);
    python_log_vargs(pLogger, "info", fmt, vargs);
    va_end(vargs);
}

void python_log_error(PyObject *pLogger, const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);
    python_log_vargs(pLogger, "error", fmt, vargs);
    va_end(vargs);
}
