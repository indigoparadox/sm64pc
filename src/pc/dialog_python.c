
#include "dialog_python.h"

#include <string.h>
#include <structmember.h>

#include "pc/logging_python.h"
#include "pc/python_helpers.h"

#include "game/ingame_menu.h"
#include "config.h"
#include "audio_defines.h"
#include "make_const_nonconst.h"
#include "game/print.h"
#include "audio/external.h"

extern PyObject *gMarioModule;

static PyObject *sLogger = NULL;

extern Gfx *gDisplayListHead;

extern u8 gHudSymCoin[];

static const Vtx vertex_text_bg_box[];
extern const Gfx dl_draw_triangle[];
extern const Gfx dl_draw_text_bg_box[];

extern s8 gDialogBoxState;
extern f32 gDialogBoxOpenTimer;
extern f32 gDialogBoxScale;
extern s16 gDialogScrollOffsetY;
extern s8 gDialogBoxType;
extern s16 gDialogID;
extern s16 gLastDialogPageStrPos;
extern s16 gDialogTextPos;
#ifdef VERSION_EU
extern s32 gInGameLanguage;
#endif
extern s8 gDialogLineNum;
extern s8 gLastDialogResponse;
extern u8 gMenuHoldKeyIndex;
extern u8 gMenuHoldKeyTimer;
extern s32 gDialogResponse;
extern f32 gDefaultSoundArgs[];
extern struct Controller *gPlayer3Controller;
extern u8 gDialogCharWidths[];

PyObject *gPyDialogPtr = NULL;

void render_dialog_box_type(struct DialogEntry *, s8 );
void render_dialog_triangle_choice(void);
u32 ensure_nonnegative(s16);
void handle_dialog_text_and_pages(s8, struct DialogEntry *, s8);
void render_dialog_string_color(s8);

#define DEFAULT_DIALOG_BOX_ANGLE 90.0f
#define DEFAULT_DIALOG_BOX_SCALE 19.0f

#if defined(VERSION_JP) || defined(VERSION_SH)
#define DIAG_VAL1 20
#define DIAG_VAL3 130
#define DIAG_VAL4 4
#else
#define DIAG_VAL1 16
#define DIAG_VAL3 132 // US & EU
#define DIAG_VAL4 5
#endif
#ifdef VERSION_EU
#define DIAG_VAL2 238
#else
#define DIAG_VAL2 240 // JP & US
#endif

/* Dialog Object */

typedef struct _PyDialogClass {
    PyObject_HEAD
    PyObject *dialog_entry;
    int dialog_type;
} PyDialogClass;

static PyMemberDef PyDialogClass_members[] = {
    {"_dialog_entry", T_OBJECT_EX, offsetof(PyDialogClass, dialog_entry), READONLY, NULL},
    {"dialog_type", T_INT, offsetof(PyDialogClass, dialog_type), 0, NULL},
    {NULL}
};

static PyObject *
PyDialog_show(PyDialogClass *self) {
    /* Ref decreased when it closes in render func below. */
    gPyDialogPtr = (PyObject *)self;
    gDialogBoxType = (s8)self->dialog_type;
    Py_INCREF(self);
    Py_RETURN_NONE;
}

static PyMethodDef PyDialogClass_methods[] = {
    {"show",    (PyCFunction)PyDialog_show,      METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static int
PyDialog_init(PyDialogClass *self, PyObject *args, PyObject *kwds) {
    const char *text = NULL;
    u8 *buf = NULL,
        dbl_quote_open = 0,
        line_w = 0;
    struct DialogEntry *dialog_entry = NULL;
    int res = 0,
        text_len = 0,
        alloc_len = 0,
        src_i = 0,
        dest_i = 0;
    //char *bhv_name = NULL;

    res = PyArg_ParseTuple(args, "s", &text);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during dialog init:");
        PyErr_Print();
        return 0;
    }

    /* +1 for the dialog text terminator. */
    text_len = strlen(text) + 1;

    dialog_entry = calloc(1, sizeof(struct DialogEntry)); 
    dialog_entry->linesPerBox = 4;
    dialog_entry->leftOffset = 30;
    dialog_entry->width = 200;

    alloc_len = text_len + 1; /* +1 for the NULL. */
    alloc_len *= 2;
    //(int)(text_len / (dialog_entry->width / 3)); /* Narrowest char is 3 wide. */

    buf = calloc(alloc_len, 1);
    //memcpy(buf, text, text_len);

    /* Copy the parsed text to the struct buffer, and while doing so,
     * translate it into the weird encoding Mario 64 uses.
     */
    for (src_i = 0;text_len > src_i;src_i++) {

        if (' ' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_SPACE;
        } else if ('\n' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_NEWLINE;
        } else if ('.' == text[src_i]) {
            buf[dest_i] = 63;
        } else if (',' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_COMMA;
        } else if ('\'' == text[src_i]) {
            buf[dest_i] = 62;
        } else if ('/' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_SLASH;
        } else if ('(' == text[src_i]) {
            buf[dest_i] = 225;
        } else if (')' == text[src_i]) {
            buf[dest_i] = 227;
        } else if ('&' == text[src_i]) {
            buf[dest_i] = 229;
        } else if ('!' == text[src_i]) {
            buf[dest_i] = 242;
        } else if ('%' == text[src_i]) {
            buf[dest_i] = 243;
        } else if ('?' == text[src_i]) {
            buf[dest_i] = 244;
        } else if ('"' == text[src_i]) {
            if (dbl_quote_open) {
                buf[dest_i] = 246;
                dbl_quote_open = 0;
            } else {
                buf[dest_i] = 245;
                dbl_quote_open = 1;
            }
        } else if ('~' == text[src_i]) {
            buf[dest_i] = 247;
        } else if ('=' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_STAR_FILLED;
        } else if ('-' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_STAR_OPEN;
        } else if ('+' == text[src_i]) {
            buf[dest_i] = 249;
        } else if ('\0' == text[src_i]) {
            buf[dest_i] = DIALOG_CHAR_TERMINATOR;
        } else if ('*' == text[src_i]) {
            buf[dest_i] = 251;
        } else {
            buf[dest_i] = ASCII_TO_DIALOG(text[src_i]);
        }

        /* Wrap at dialog edge. Good enough for now. */
        // TODO: Break at spaces.
        assert(buf[dest_i] < 256);
        line_w += (int)(gDialogCharWidths[buf[dest_i]] * 1.9);
        if (line_w >= dialog_entry->width) {
            dest_i += 1;
            buf[dest_i] = DIALOG_CHAR_NEWLINE;
            line_w = 0;
        }

        dest_i++;
    }

    dialog_entry->str = buf;

    self->dialog_entry = PYTHON_ENCAPSULE(dialog_entry, PYCAPSULE_TYPE_DIALOG, sLogger, ;);

    self->dialog_type = (int)DIALOG_TYPE_ROTATE;

    return 0;
}

void
PyDialog_destroy(PyDialogClass *self) {
    struct DialogEntry *dialog_entry = NULL;

    dialog_entry = PYTHON_DECAPSULE(
        self->dialog_entry,
        PYCAPSULE_TYPE_DIALOG,
        struct DialogEntry, sLogger, return);

    free((void*)dialog_entry->str);
    free((void*)dialog_entry);

    Py_DECREF(self->dialog_entry);
}

static PyTypeObject PyDialogType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "dialog.Dialog",
    .tp_basicsize = sizeof(PyDialogClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_methods = PyDialogClass_methods,
    .tp_members = PyDialogClass_members,
    .tp_init = (initproc)PyDialog_init,
    .tp_dealloc = (destructor)PyDialog_destroy,
};

/* Dialog Module */

static PyObject *
PyDialog_print_colorful_text(PyObject *self, PyObject *args) {
    s16 x = 0,
        y = 0;
    const char *str;
    int res = 0;

    res = PyArg_ParseTuple(args, "hhy", &x, &y, &str);
    if (!res || PyErr_Occurred()) {
        python_log_error(sLogger, "during print:");
        PyErr_Print();
        Py_RETURN_NONE;
    }

    print_text(x, y, str);

    Py_RETURN_NONE;
}

static PyMethodDef PyDialog_methods[] = {
    {"print_colorful_text",   (PyCFunction)PyDialog_print_colorful_text,   METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef PyDialogModule = {
    PyModuleDef_HEAD_INIT, "dialog", NULL, -1, PyDialog_methods,
    NULL, NULL, NULL, NULL
};

PyObject* PyInit_dialog(void) {
    PyObject *pDialog;

    if (NULL == sLogger) {
        sLogger = python_get_logger("dialog");
    }

    if(0 > PyType_Ready( &PyDialogType)) {
        python_log_error(sLogger, "Dialog type not ready?");
        return NULL;
    }

    pDialog = PyModule_Create(&PyDialogModule);
    if(NULL == pDialog) {
        python_log_error(sLogger, "could not allocate dialog module");
        return NULL;
    }

    Py_INCREF(&PyDialogType);
    if( 0 > PyModule_AddObject(pDialog, "Dialog", (PyObject *)&PyDialogType)) {
        python_log_error(sLogger, "unable to add Dialog to dialog module");
        Py_DECREF(&PyDialogType);
        Py_DECREF(pDialog);
        return NULL;
    }

    PYTHON_DIALOG_ADD_CONSTANTS(pDialog);

    python_log_debug(sLogger, "dialog module initialized");

    return pDialog;
}

void dialog_python_render_frame() {
    PyObject *pFunc = NULL;

    pFunc = PyObject_GetAttrString(gMarioModule, "dialog_render_frame");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject_CallObject(pFunc, NULL);
    }

    if (PyErr_Occurred()) {
        PyErr_Print();
    }
   
    Py_XDECREF(pFunc);
}

void dialog_python_render_dialog() {
    struct DialogEntry *dialog;
    s8 lowerBound;
    gDialogID = -1;
    dialog = PYTHON_DECAPSULE(
        ((PyDialogClass *)gPyDialogPtr)->dialog_entry,
        PYCAPSULE_TYPE_DIALOG,
        struct DialogEntry, sLogger, return);

    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                //play_dialog_sound(gDialogID);
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gDefaultSoundArgs);
            }

            if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
                gDialogBoxOpenTimer -= 7.5;
                gDialogBoxScale -= 1.5;
            } else {
                gDialogBoxOpenTimer -= 10.0;
                gDialogBoxScale -= 2.0;
            }

            if (gDialogBoxOpenTimer == 0.0f) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogLineNum = 1;
            }
            lowerBound = 1;
            break;
        case DIALOG_STATE_VERTICAL:
            gDialogBoxOpenTimer = 0.0f;

            if ((gPlayer3Controller->buttonPressed & A_BUTTON)
                || (gPlayer3Controller->buttonPressed & B_BUTTON)) {
                if (gLastDialogPageStrPos == -1) {
                    //handle_special_dialog_text(gDialogID);
                    gDialogBoxState = DIALOG_STATE_CLOSING;
                } else {
                    gDialogBoxState = DIALOG_STATE_HORIZONTAL;
                    play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gDefaultSoundArgs);
                }
            }
            lowerBound = 1;
            break;
        case DIALOG_STATE_HORIZONTAL:
            gDialogScrollOffsetY += dialog->linesPerBox * 2;

            if (gDialogScrollOffsetY >= dialog->linesPerBox * DIAG_VAL1) {
                gDialogTextPos = gLastDialogPageStrPos;
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogScrollOffsetY = 0;
            }
            lowerBound = (gDialogScrollOffsetY / 16) + 1;
            break;
        case DIALOG_STATE_CLOSING:
            if (gDialogBoxOpenTimer == 20.0f) {
                //level_set_transition(0, 0);
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gDefaultSoundArgs);

                #if 0
                if (gDialogBoxType == DIALOG_TYPE_ZOOM) {
                    trigger_cutscene_dialog(2);
                }
                #endif

                gDialogResponse = gDialogLineNum;
            }

            gDialogBoxOpenTimer = gDialogBoxOpenTimer + 10.0f;
            gDialogBoxScale = gDialogBoxScale + 2.0f;

            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                gDialogBoxState = DIALOG_STATE_OPENING;
                gDialogID = -1;
                gDialogTextPos = 0;
                gLastDialogResponse = 0;
                gLastDialogPageStrPos = 0;
                gDialogResponse = 0;
                Py_DECREF(gPyDialogPtr);
                gPyDialogPtr = NULL;
            }
            lowerBound = 1;
            break;
    }

    render_dialog_box_type(dialog, dialog->linesPerBox);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE,
                  0,
                  ensure_nonnegative(DIAG_VAL2 - dialog->width),
                  SCREEN_WIDTH,
                  ensure_nonnegative(240 + ((dialog->linesPerBox * 80) / DIAG_VAL4) - dialog->width));
    handle_dialog_text_and_pages(0, dialog, lowerBound);

    if (gLastDialogPageStrPos == -1 && gLastDialogResponse == 1) {
        render_dialog_triangle_choice();
    }
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 2, 2, SCREEN_WIDTH - BORDER_HEIGHT/2, SCREEN_HEIGHT - BORDER_HEIGHT/2);
    if (gLastDialogPageStrPos != -1 && gDialogBoxState == DIALOG_STATE_VERTICAL) {
        render_dialog_string_color(dialog->linesPerBox);
    }
}
