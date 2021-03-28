
#include "save_file_python.h"

#include "game/save_file.h"

/* SaveFile Module */



static PyMethodDef PySaveFile_methods[] = {
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

    fprintf(stdout, "save_file module initialized\n");

    return pSaveFile;
}
