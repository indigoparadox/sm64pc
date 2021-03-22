

#include <Python.h>
#include <unistd.h>
#include <structmember.h>

#include "sm64.h"
#include "game/mario.h"

PyObject *gMarioModule;

#ifdef USE_PYTHON

typedef struct {
    PyObject_HEAD
    u16 unk00;
    u16 input;
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
    struct Surface *wall;
    struct Surface *ceil;
    struct Surface *floor;
    f32 ceilHeight;
    f32 floorHeight;
    s16 floorAngle;
    s16 waterLevel;
    struct Object *interactObj;
    struct Object *heldObj;
    struct Object *usedObj;
    struct Object *riddenObj;
    struct Object *marioObj;
    struct SpawnInfo *spawnInfo;
    struct Area *area;
    struct PlayerCameraState *statusForCamera;
    struct MarioBodyState *marioBodyState;
    struct Controller *controller;
    struct MarioAnimation *animation;
    u32 collidedObjInteractTypes;
    s16 numCoins;
    s16 numStars;
    s8 numKeys;
    s8 numLives;
    s16 health;
    s16 unkB0;
    u8 hurtCounter;
    u8 healCounter;
    u8 squishTimer;
    u8 fadeWarpOpacity;
    u16 capTimer;
    f32 peakHeight;
    s16 unkB8;
    f32 quicksandDepth;
    f32 unkC4;
} MarioPythonObject;

static PyMemberDef MarioPythonMembers[] = {
    {"unk00",                       T_USHORT,   offsetof(MarioPythonObject, unk00), 0, NULL},
    {"input",                       T_USHORT,   offsetof(MarioPythonObject, input), 0, NULL},
    {"flags",                       T_ULONG,    offsetof(MarioPythonObject, flags), 0, "flags"},
    {"particleFlags",               T_ULONG,    offsetof(MarioPythonObject, particleFlags), 0, "particleFlags"},
    {"action",                      T_ULONG,    offsetof(MarioPythonObject, action), 0, "action"},
    {"prevAction",                  T_ULONG,    offsetof(MarioPythonObject, prevAction), 0, "prevAction"},
    {"terrainSoundAddend",          T_ULONG,    offsetof(MarioPythonObject, terrainSoundAddend), 0, "terrainSoundAddend"},
    {"actionState",                 T_USHORT,   offsetof(MarioPythonObject, actionState), 0, "actionState"},
    {"actionTimer",                 T_USHORT,   offsetof(MarioPythonObject, actionTimer), 0, "actionTimer"},
    {"actionArg",                   T_ULONG,    offsetof(MarioPythonObject, actionArg), 0, "actionArg"},
    {"intendedMag",                 T_FLOAT,    offsetof(MarioPythonObject, intendedMag), 0, "intendedMag"},
    {"intendedYaw",                 T_SHORT,    offsetof(MarioPythonObject, intendedYaw), 0, "intendedYaw"},
    {"invincTimer",                 T_SHORT,    offsetof(MarioPythonObject, invincTimer), 0, "invincTimer"},
    {"framesSinceA",                T_UBYTE,    offsetof(MarioPythonObject, framesSinceA), 0, "framesSinceA"},
    {"framesSinceB",                T_UBYTE,    offsetof(MarioPythonObject, framesSinceB), 0, "framesSinceB"},
    {"wallKickTimer",               T_UBYTE,    offsetof(MarioPythonObject, wallKickTimer), 0, "wallKickTimer"},
    {"doubleJumpTimer",             T_UBYTE,    offsetof(MarioPythonObject, doubleJumpTimer), 0, "doubleJumpTimer"},
    //
    {"slideYaw",                    T_SHORT,    offsetof(MarioPythonObject, slideYaw), 0, "slideYaw"},
    {"twirlYaw",                    T_SHORT,    offsetof(MarioPythonObject, twirlYaw), 0, "twirlYaw"},
    //
    {"forwardVel",                  T_FLOAT,    offsetof(MarioPythonObject, forwardVel), 0, "forwardVel"},
    {"slideVelX",                   T_FLOAT,    offsetof(MarioPythonObject, slideVelX), 0, "slideVelX"},
    {"slideVelZ",                   T_FLOAT,    offsetof(MarioPythonObject, slideVelZ), 0, "slideVelZ"},
    {"wall",                        T_OBJECT,   offsetof(MarioPythonObject, wall), 0, NULL},
    {"ceil",                        T_OBJECT,   offsetof(MarioPythonObject, ceil), 0, NULL},
    {"floor",                       T_OBJECT,   offsetof(MarioPythonObject, floor), 0, NULL},
    {"ceilHeight",                  T_FLOAT,    offsetof(MarioPythonObject, ceilHeight), 0, "ceilHeight"},
    {"floorHeight",                 T_FLOAT,    offsetof(MarioPythonObject, floorHeight), 0, "floorHeight"},
    {"floorAngle",                  T_SHORT,    offsetof(MarioPythonObject, floorAngle), 0, "floorAngle"},
    {"waterLevel",                  T_SHORT,    offsetof(MarioPythonObject, waterLevel), 0, "waterLevel"},
    {"interactObj",                 T_OBJECT,   offsetof(MarioPythonObject, interactObj), 0, NULL},
    {"heldObj",                     T_OBJECT,   offsetof(MarioPythonObject, heldObj), 0, NULL},
    {"usedObj",                     T_OBJECT,   offsetof(MarioPythonObject, usedObj), 0, NULL},
    {"riddenObj",                   T_OBJECT,   offsetof(MarioPythonObject, riddenObj), 0, NULL},
    {"marioObj",                    T_OBJECT,   offsetof(MarioPythonObject, marioObj), 0, NULL},
    {"spawnInfo",                   T_OBJECT,   offsetof(MarioPythonObject, spawnInfo), 0, NULL},
    {"area",                        T_OBJECT,   offsetof(MarioPythonObject, area), 0, NULL},
    {"statusForCamera",             T_OBJECT,   offsetof(MarioPythonObject, statusForCamera), 0, NULL},
    {"marioBodyState",              T_OBJECT,   offsetof(MarioPythonObject, marioBodyState), 0, NULL},
    {"controller",                  T_OBJECT,   offsetof(MarioPythonObject, controller), 0, NULL},
    {"animation",                   T_OBJECT,   offsetof(MarioPythonObject, animation), 0, NULL},
    {"collidedObjInteractTypes",    T_ULONG,    offsetof(MarioPythonObject, collidedObjInteractTypes), 0, "collidedObjInteractTypes"},
    {"numCoins",                    T_SHORT,    offsetof(MarioPythonObject, numCoins), 0, "numCoins"},
    {"numStars",                    T_SHORT,    offsetof(MarioPythonObject, numStars), 0, "numStars"},
    {"numKeys",                     T_UBYTE,    offsetof(MarioPythonObject, numKeys), 0, NULL},
    {"numLives",                    T_UBYTE,    offsetof(MarioPythonObject, numLives), 0, "numLives"},
    {"health",                      T_SHORT,    offsetof(MarioPythonObject, health), 0, "health"},
    //
    {"hurtCounter",                 T_UBYTE,    offsetof(MarioPythonObject, hurtCounter), 0, "hurtCounter"},
    {"healCounter",                 T_UBYTE,    offsetof(MarioPythonObject, healCounter), 0, "healCounter"},
    {"squishTimer",                 T_UBYTE,    offsetof(MarioPythonObject, squishTimer), 0, "squishTimer"},
    {"fadeWarpOpacity",             T_UBYTE,    offsetof(MarioPythonObject, fadeWarpOpacity), 0, "fadeWarpOpacity"},
    {"capTimer",                    T_USHORT,   offsetof(MarioPythonObject, capTimer), 0, "capTimer"},
    //
    {"peakHeight",                  T_FLOAT,    offsetof(MarioPythonObject, peakHeight), 0, "peakHeight"},    
    {"quicksandDepth",              T_FLOAT,    offsetof(MarioPythonObject, quicksandDepth), 0, "quicksandDepth"},
    //
    {NULL},
};

static PyMethodDef MarioPythonMethods[] = {
    //{ "set",    (PyCFunction) CountDict_set, METH_VARARGS,
    //           "Set a key and increment the count." },
    {NULL, NULL, 0, NULL}
};

static PyTypeObject MarioPythonType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "mario.Mario",
    .tp_doc = "Mario state objects",
    .tp_basicsize = sizeof(MarioPythonObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_members = MarioPythonMembers,
};

/*
static PyObject *MarioPythonObject_set(MarioPythonObject *self, PyObject *args) {
   const char *key;
   PyObject *value;

   if (!PyArg_ParseTuple(args, "sO:set", &key, &value)) {
      return NULL;
   }

   if (PyDict_SetItemString(self->dict, key, value) < 0) {
      return NULL;
   }

   self->count++;

   return Py_BuildValue("i", self->count);
}
*/

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

    /* Call the class object. */
    //PyObject *obj = PyObject_CallObject((PyObject *) &MarioPythonType, NULL);

    //PyModule_AddObject( pMario, "marioState", )

    /* Release the argument list. */
    //Py_DECREF(argList);

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