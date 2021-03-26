
#include <check.h>

#include <Python.h>

#include "../src/pc/mario_python.h"
#include "../src/pc/mario_python.h"
#include "../src/pc/object_python.h"
#include "../src/pc/level_python.h"
#include "../src/pc/dialog_python.h"

#include "sm64.h"

#include "game/memory.h"
#include "audio/external.h"

#include "gfx/gfx_pc.h"
#include "gfx/gfx_opengl.h"
#include "gfx/gfx_direct3d11.h"
#include "gfx/gfx_direct3d12.h"
#include "gfx/gfx_sdl.h"

#include "audio/audio_api.h"
#include "audio/audio_sdl.h"
#include "audio/audio_null.h"

#include "pc_main.h"
#include "cliopts.h"
#include "configfile.h"
#include "controller/controller_api.h"
#include "controller/controller_keyboard.h"
#include "fs/fs.h"

#include "game/game_init.h"
#include "game/main.h"
#include "game/thread6.h"


OSMesg D_80339BEC;
OSMesgQueue gSIEventMesgQueue;

s8 gResetTimer;
s8 D_8032C648;
s8 gDebugLevelSelect;
s8 gShowProfiler;
s8 gShowDebugText;

s32 gRumblePakPfs;
struct RumbleData gRumbleDataQueue[3];
struct StructSH8031D9B0 gCurrRumbleSettings;

static struct AudioAPI *audio_api;
static struct GfxWindowManagerAPI *wm_api;
static struct GfxRenderingAPI *rendering_api;

extern void gfx_run(Gfx *commands);
extern void thread5_game_loop(void *arg);
extern void create_next_audio_buffer(s16 *samples, u32 num_samples);
void game_loop_one_iteration(void);

extern PyObject *gMarioModule;

/* This is the hook called by main() to get ALL of python running. */
void python_init() {
    PyObject *pName, *pSysPath, *pCwd, *pFunc;
    char cwd[255];

    Py_SetProgramName( Py_DecodeLocale( "sm64pc", NULL ) );
    if (0 > PyImport_AppendInittab("mario", &PyInit_mario)) {
        fprintf(stderr, "could not add mario to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("objects", &PyInit_objects)) {
        fprintf(stderr, "could not add objects to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("levels", &PyInit_levels)) {
        fprintf(stderr, "could not add levels to inittab\n");
        return;
    }
    if (0 > PyImport_AppendInittab("dialog", &PyInit_dialog)) {
        fprintf(stderr, "could not add levels to inittab\n");
        return;
    }
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
    if (PyErr_Occurred()) {
        fprintf( stderr, "during setup:\n" );
        PyErr_Print();
        return;
    }

    pFunc = PyObject_GetAttrString(gMarioModule, "mario_init");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject_CallObject(pFunc, NULL);
        if (PyErr_Occurred()) {
            fprintf( stderr, "during init:\n" );
            PyErr_Print();
        }
    }

    assert( NULL != gMarioModule );
}

void dispatch_audio_sptask(struct SPTask *spTask) {
}

void set_vblank_handler(s32 index, struct VblankHandler *handler, OSMesgQueue *queue, OSMesg *msg) {
}

static bool inited = false;

void send_display_list(struct SPTask *spTask) {
}

void produce_one_frame(void) {
}

void audio_shutdown(void) {
}

void game_deinit(void) {
}

void game_exit(void) {
}

void main_func(void) {
}

int main(int argc, char *argv[]) {
    return 0;
}

