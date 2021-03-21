

#ifdef USE_PYTHON
#include <Python.h>
#include <unistd.h>
#endif /* USE_PYTHON */

#include "sm64.h"
#include "game/mario.h"

#ifdef USE_PYTHON
PyObject *gMarioModule;
#endif /* USE_PYTHON */

#ifdef USE_PYTHON

typedef struct {
    PyObject_HEAD
    u32 flags;
    u32 particleFlags;
    u32 action;
    u32 prevAction;
    u32 terrainSoundAddend;
    u16 actionState;
    u16 actionTimer;
    u32 actionArg;
    f32 intendedMag;
    s16 intendedYaw;
    s16 invincTimer;
    u8 framesSinceA;
    u8 framesSinceB;
    u8 wallKickTimer;
    u8 doubleJumpTimer;
    //Vec3s faceAngle;
    //Vec3s angleVel;
    s16 slideYaw;
    s16 twirlYaw;
    //Vec3f pos;
    //Vec3f vel;
    f32 forwardVel;
    f32 slideVelX;
    f32 slideVelZ;
    f32 ceilHeight;
    f32 floorHeight;
    s16 floorAngle;
    s16 waterLevel;
    u32 collidedObjInteractTypes;
    s16 numCoins;
    s16 numStars;
    s8 numLives;
    s16 health;
    u8 hurtCounter;
    u8 healCounter;
    u8 squishTimer;
    u8 fadeWarpOpacity;
    u16 capTimer;
    f32 peakHeight;
    f32 quicksandDepth;
} MarioPythonObject;

static PyTypeObject MarioPythonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "mario.Mario",
    .tp_doc = "Mario state objects",
    .tp_basicsize = sizeof(MarioPythonObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};

static PyMethodDef MarioMethods[] = {
    {NULL, NULL, 0, NULL}
};

static PyModuleDef MarioModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, MarioMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_mario(void) {

    if (PyType_Ready(&MarioPythonType) < 0) {
        return NULL;
    }

    PyObject *pMario = PyModule_Create(&MarioModule);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_MASK", ACT_GROUP_MASK);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_STATIONARY", ACT_GROUP_STATIONARY);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_MOVING", ACT_GROUP_MOVING);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_AIRBORNE", ACT_GROUP_AIRBORNE);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_SUBMERGED", ACT_GROUP_SUBMERGED);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_CUTSCENE", ACT_GROUP_CUTSCENE);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_AUTOMATIC", ACT_GROUP_AUTOMATIC);
    PyModule_AddIntConstant(pMario, "ACT_GROUP_OBJECT", ACT_GROUP_OBJECT);
    PyModule_AddIntConstant(pMario, "MARIO_ACTION_SOUND_PLAYED", MARIO_ACTION_SOUND_PLAYED);
    PyModule_AddIntConstant(pMario, "MARIO_MARIO_SOUND_PLAYED", MARIO_MARIO_SOUND_PLAYED);
    PyModule_AddIntConstant(pMario, "ACT_FLAG_AIR", ACT_FLAG_AIR);
    PyModule_AddIntConstant(pMario, "MARIO_UNKNOWN_18", MARIO_UNKNOWN_18);

    Py_INCREF(&MarioPythonType);
    if (PyModule_AddObject(pMario, "Mario", (PyObject *) &MarioPythonType) < 0) {
        Py_DECREF(&MarioPythonType);
        Py_DECREF(pMario);
        return NULL;
    }

    return pMario;
}

void python_init() {
    PyObject *pName, *pSysPath, *pCwd;
    char cwd[255];

    Py_SetProgramName( Py_DecodeLocale( "sm64pc", NULL ) );
    PyImport_AppendInittab("mario", &PyInit_mario);
    Py_Initialize();
    fprintf(stdout, "python initialized\n");

    /* Load the mario module. */
    /* TODO: Load from user dir? */
    pSysPath = PySys_GetObject((char*)"path");
    getcwd(cwd, 255);
    pCwd = PyUnicode_FromString(cwd);
    PyList_Append(pSysPath, pCwd);
    Py_DECREF(pCwd);
    pName = PyUnicode_DecodeFSDefault( "sm64" );
    /* TODO: Error checking of pName left out */

    gMarioModule = PyImport_Import( pName );

    assert( NULL != gMarioModule );

    fprintf(stdout, "mario module loaded\n");
}

#endif /* USE_PYTHON */