
#include "sounds_python.h"

#include "logging_python.h"

#include "audio/external.h"

PyObject *sLogger = NULL;

/* Sound Module */

static PyObject *
PySounds_play_sound(PyObject *self, PyObject *args) {
    PyObject *pPosTuple = NULL,
        *pPosNum = NULL;
    s32 sound_bits = 0;
    f32 pos[3] = {0.0f, 0.0f, 0.0f};
    int res = 0,
        i = 0;

    res = PyArg_ParseTuple(args, "lO", &sound_bits, &pPosTuple);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during play sound:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    for (i = 0;3 > i;i++) {
        pPosNum = PyTuple_GetItem(pPosTuple, i);
        if (PyErr_Occurred()) {
            python_log_error(sLogger, "during play sound:");
            PyErr_Print();
            Py_RETURN_NONE;
        }
        pos[i] = PyFloat_AsDouble(pPosNum);
        if (PyErr_Occurred()) {
            python_log_error(sLogger, "during play sound:");
            PyErr_Print();
            Py_RETURN_NONE;
        }
        Py_DECREF(pPosNum);
    }

    play_sound(sound_bits, pos);

    Py_RETURN_NONE;
}

static PyMethodDef PySounds_methods[] = {
    {"play_sound",     (PyCFunction)PySounds_play_sound,    METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PySoundsModule = {
    PyModuleDef_HEAD_INIT, "sound", NULL, -1, PySounds_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_sounds(void) {
    PyObject *pSounds = NULL;

    if (NULL == sLogger) {
        sLogger = python_get_logger("sounds");
    }

    pSounds = PyModule_Create(&PySoundsModule);
    if(NULL == pSounds) {
        python_log_error(sLogger, "could not allocate sounds module" );
        return NULL;
    }

    PYTHON_SOUND_ADD_CONSTANTS(pSounds);

    python_log_debug(sLogger, "sounds module initialized");

    return pSounds;
}
