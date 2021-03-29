
#include "save_file_python.h"

#include "game/save_file.h"

/* SaveFile Module */

static PyObject *
PySaveFile_collect_star_or_key(PyObject *self, PyObject *args) {
    s16 num_coins = 0,
        star_index = 0;
    int res = 0;

    res = PyArg_ParseTuple(args, "hh", &num_coins, &star_index);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during save file collect star or key:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    save_file_collect_star_or_key(num_coins, star_index);
    
    Py_RETURN_NONE;
}

static PyObject *
PySaveFile_get_total_star_count(PyObject *self, PyObject *args) {
    s32 file_index = 0,
        min_course = 0,
        max_course = 0,
        total_star_count = 0;
    PyObject *pTotalStarCount = NULL;
    int res = 0;

    res = PyArg_ParseTuple(args, "iii", &file_index, &min_course, &max_course);
    if (!res || PyErr_Occurred()) {
        fprintf(stderr, "during save file get total star count:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    total_star_count = save_file_get_total_star_count(file_index, min_course, max_course);
    
    pTotalStarCount = PyLong_FromLong(total_star_count);
    if (PyErr_Occurred()) {
        fprintf(stderr, "during save file get total star count:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }
    
    return pTotalStarCount;
}

static PyObject *
PySaveFile_get_curr_save_file_num(PyObject *self) {
    PyObject *pCurrSaveFileNum = NULL;

    pCurrSaveFileNum = PyLong_FromLong(gCurrSaveFileNum);
    if (PyErr_Occurred()) {
        fprintf(stderr, "during get curr save file num:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pCurrSaveFileNum;
}

static PyObject *
PySaveFile_get_flags(PyObject *self) {
    PyObject *pFlags = NULL;

    pFlags = PyLong_FromUnsignedLong(save_file_get_flags());
    if (PyErr_Occurred()) {
        fprintf(stderr, "during get save file flags:\n");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    return pFlags;
}

static PyMethodDef PySaveFile_methods[] = {
    {"collect_star_or_key",     (PyCFunction)PySaveFile_collect_star_or_key,    METH_VARARGS, NULL},
    {"get_total_star_count",    (PyCFunction)PySaveFile_get_total_star_count,   METH_VARARGS, NULL},
    {"get_curr_save_file_num",  (PyCFunction)PySaveFile_get_curr_save_file_num, METH_NOARGS, NULL},
    {"get_flags",               (PyCFunction)PySaveFile_get_flags,              METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PySaveFileModule = {
    PyModuleDef_HEAD_INIT, "save_file", NULL, -1, PySaveFile_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_save_file(void) {
    PyObject *pSaveFile;

    pSaveFile = PyModule_Create(&PySaveFileModule);
    if(NULL == pSaveFile) {
        fprintf( stderr, "could not allocate save_file module\n" );
        return NULL;
    }

    PyModule_AddIntConstant(pSaveFile, "COURSE_MIN", COURSE_MIN);
    PyModule_AddIntConstant(pSaveFile, "COURSE_MAX", COURSE_MAX);

    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_FILE_EXISTS", SAVE_FLAG_FILE_EXISTS);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_HAVE_WING_CAP", SAVE_FLAG_HAVE_WING_CAP);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_HAVE_METAL_CAP", SAVE_FLAG_HAVE_METAL_CAP);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_HAVE_VANISH_CAP", SAVE_FLAG_HAVE_VANISH_CAP);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_HAVE_KEY_1", SAVE_FLAG_HAVE_KEY_1);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_HAVE_KEY_2", SAVE_FLAG_HAVE_KEY_2);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_BASEMENT_DOOR", SAVE_FLAG_UNLOCKED_BASEMENT_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR", SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_DDD_MOVED_BACK", SAVE_FLAG_DDD_MOVED_BACK);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_MOAT_DRAINED", SAVE_FLAG_MOAT_DRAINED);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_PSS_DOOR", SAVE_FLAG_UNLOCKED_PSS_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_WF_DOOR", SAVE_FLAG_UNLOCKED_WF_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_CCM_DOOR", SAVE_FLAG_UNLOCKED_CCM_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_JRB_DOOR", SAVE_FLAG_UNLOCKED_JRB_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_BITDW_DOOR", SAVE_FLAG_UNLOCKED_BITDW_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_BITFS_DOOR", SAVE_FLAG_UNLOCKED_BITFS_DOOR);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_CAP_ON_GROUND", SAVE_FLAG_CAP_ON_GROUND);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_CAP_ON_KLEPTO", SAVE_FLAG_CAP_ON_KLEPTO);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_CAP_ON_UKIKI", SAVE_FLAG_CAP_ON_UKIKI);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_CAP_ON_MR_BLIZZARD", SAVE_FLAG_CAP_ON_MR_BLIZZARD);
    PyModule_AddIntConstant(pSaveFile, "SAVE_FLAG_UNLOCKED_50_STAR_DOOR", SAVE_FLAG_UNLOCKED_50_STAR_DOOR);

    fprintf(stdout, "save_file module initialized\n");

    return pSaveFile;
}
