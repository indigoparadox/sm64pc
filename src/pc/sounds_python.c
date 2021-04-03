
#include "sounds_python.h"

#include "logging_python.h"

#include "audio/external.h"
#include "game/sound_init.h"

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

static PyObject *
PySounds_play_cap_music(PyObject *self, PyObject *args) {
    u8 priority = 0;
    s16 cap_music = 0;
    int res = 0;

    res = PyArg_ParseTuple(args, "bh", &priority, &cap_music);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during play cap music:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    play_cap_music(SEQUENCE_ARGS(priority, cap_music));

    Py_RETURN_NONE;
}

static PyObject *
PySounds_play_shell_music(PyObject *self) {

    play_shell_music();

    Py_RETURN_NONE;
}

static PyObject *
PySounds_drop_queued_background_music(PyObject *self) {

    drop_queued_background_music();

    Py_RETURN_NONE;
}

static PyObject *
PySounds_fadeout_level_music(PyObject *self, PyObject *args) {
    s16 fade_timer = 0;
    int res = 0;

    res = PyArg_ParseTuple(args, "h", &fade_timer);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during play cap music:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    fadeout_level_music(fade_timer);

    Py_RETURN_NONE;
}

static PyMethodDef PySounds_methods[] = {
    {"play_sound",     (PyCFunction)PySounds_play_sound,    METH_VARARGS, NULL},
    {"play_cap_music",     (PyCFunction)PySounds_play_cap_music,    METH_VARARGS, NULL},
    {"play_shell_music",     (PyCFunction)PySounds_play_shell_music,    METH_NOARGS, NULL},
    {"drop_queued_background_music", (PyCFunction)PySounds_drop_queued_background_music, METH_NOARGS, NULL},
    {"fadeout_level_music",     (PyCFunction)PySounds_fadeout_level_music,    METH_VARARGS, NULL},
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
