
#include "camera_python.h"

#include <Python.h>
#include <structmember.h>

#include "game/camera.h"
#include "goddard/gd_types.h"
#include "engine/graph_node.h"

#include "logging_python.h"
#include "python_helpers.h"

static PyObject *sLogger = NULL;

s32 set_camera_mode_fixed(struct Camera *, s16, s16, s16);

typedef struct _PyCameraClass {
    PyObject_HEAD
    PyObject *native_camera;
} PyCameraClass;

static PyMemberDef PyCameraObj_members[] = {
    {"_native_camera", T_OBJECT_EX, offsetof(PyCameraClass, native_camera), READONLY, NULL},
    {NULL}
};

static PyObject *
PyCameraObj_set_camera_mode_fixed(PyCameraClass* self, PyObject *args) {
    s16 x=0, y=0, z=0;
    int res = 0;
    struct Camera *native_camera = NULL;

    res = PyArg_ParseTuple( args, "hhh", &x, &y, &z);
    if (!res || PyErr_Occurred()) { \
        python_log_error(sLogger, "during set camera mode fixed:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    native_camera = PYTHON_DECAPSULE(
        self->native_camera,
        PYCAPSULE_TYPE_CAMERA,
        struct Camera, sLogger,
        Py_RETURN_NONE);

    set_camera_mode_fixed(native_camera, x, y, z);

    Py_RETURN_NONE;
}

static PyMethodDef PyCameraObj_methods[] = {
    {"set_camera_mode_fixed",       (PyCFunction)PyCameraObj_set_camera_mode_fixed,        METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static int
PyCameraObj_init(PyCameraClass *self, PyObject *args, PyObject *kwds) {
    PyObject *pNativeCameraCap = NULL;
    int res = 0;

    res = PyArg_ParseTuple(args, "|O", &pNativeCameraCap);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during camera init:");
        PyErr_Print();
        return 0;
    }
    
    if (NULL != pNativeCameraCap) {
        self->native_camera = pNativeCameraCap;
    } else {
        self->native_camera = (PyObject *)Py_None;
    }
    Py_INCREF(self->native_camera);

    return 0;
}

void
PyCameraObj_destroy(PyCameraClass *self) {
    Py_DECREF(self->native_camera);
}

static PyTypeObject PyCameraType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cameras.Camera",
    .tp_basicsize = sizeof(PyCameraClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_methods = PyCameraObj_methods,
    .tp_members = PyCameraObj_members,
    .tp_dealloc = (destructor)PyCameraObj_destroy,
    .tp_init = (initproc)PyCameraObj_init,
};

/* Camera Module */

PYTHON_WRAP_FLAGS(gCameraMovementFlag, gCameraMovementFlags, PyCamera, sLogger);

static PyObject *
PyCamera_set_shake_from_hit(PyObject *self, PyObject *arg) {
    s16 shake = 0;

    shake = PyLong_AsLong(arg);
    set_camera_shake_from_hit(shake);

    Py_RETURN_NONE;
}

static PyMethodDef PyCamera_methods[] = {
    {"unset_movement_flag",  (PyCFunction)PyCamera_unset_gCameraMovementFlag,    METH_O, NULL},
    {"set_movement_flag",    (PyCFunction)PyCamera_set_gCameraMovementFlag,      METH_O, NULL},
    {"set_shake_from_hit",  (PyCFunction)PyCamera_set_shake_from_hit,    METH_O, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PyCameraModule = {
    PyModuleDef_HEAD_INIT, "cameras", NULL, -1, PyCamera_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_cameras(void) {
    PyObject *pCamera;

    if (NULL == sLogger) {
        sLogger = python_get_logger("cameras");
    }

    if(0 > PyType_Ready( &PyCameraType)) {
        python_log_error(sLogger, "type not ready?" );
        return NULL;
    }

    pCamera = PyModule_Create(&PyCameraModule);
    if(NULL == pCamera) {
        python_log_error(sLogger, "could not allocate camera module" );
        return NULL;
    }

    Py_INCREF(&PyCameraType);
    if( 0 > PyModule_AddObject(pCamera, "Camera", (PyObject *)&PyCameraType)) {
        python_log_error(sLogger, "unable to add Camera to cameras module");
        Py_DECREF(&PyCameraType);
        Py_DECREF(pCamera);
        return NULL;
    }

    PYTHON_CAMERA_ADD_CONSTANTS(pCamera);

    python_log_info(sLogger, "cameras module initialized");

    return pCamera;
}

void python_camera_wrap(struct Camera *cam_in) {
    PyObject *pNativeCameraCap = NULL,
        *pArgs = NULL,
        *pCamera = NULL;
        
    pNativeCameraCap = PYTHON_ENCAPSULE(cam_in, PYCAPSULE_TYPE_CAMERA, sLogger, ;);
    
    pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, pNativeCameraCap);
    
    Py_INCREF(&PyCameraType);
    pCamera = PyObject_CallObject(
        (PyObject *)&PyCameraType, pArgs);

    cam_in->pyCameraState = (PyCameraClass *)pCamera;
    Py_DECREF(pArgs);
}

void python_camera_test_and_remove_state(struct GraphNode *g_node) {
    struct GraphNodeCamera *gn_camera = NULL;
    struct Camera *camera_native = NULL;
    PyCameraClass *camera = NULL;

    if ((g_node->type & GRAPH_NODE_TYPE_CAMERA) != GRAPH_NODE_TYPE_CAMERA) {
        return;
    }

    gn_camera = (struct GraphNodeCamera *)g_node;

    if (geo_camera_main != gn_camera->fnNode.func) {
        return;
    }

    camera_native = gn_camera->config.camera;

    if (NULL == camera_native) {
        return;
    }

    /* TODO: Nothing makes it this far. */

    camera = camera_native->pyCameraState;
        
    if (PyCapsule_IsValid(camera->native_camera, PYCAPSULE_TYPE_CAMERA)) {
        fprintf(stdout, "removing camera\n");
        Py_DECREF(camera);
    }
}
