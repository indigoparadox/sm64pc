
#include "level_table.h"

#define LEVEL_CONSTANT(module, level) \
    PyModule_AddIntConstant(module, #level, level);

#define ADD_LEVEL_CONSTANTS(module) \
    LEVEL_CONSTANT(module, LEVEL_BBH) \
    LEVEL_CONSTANT(module, LEVEL_CCM) \
    LEVEL_CONSTANT(module, LEVEL_CASTLE) \
    LEVEL_CONSTANT(module, LEVEL_HMC) \
    LEVEL_CONSTANT(module, LEVEL_SSL) \
    LEVEL_CONSTANT(module, LEVEL_BOB) \
    LEVEL_CONSTANT(module, LEVEL_SL) \
    LEVEL_CONSTANT(module, LEVEL_WDW) \
    LEVEL_CONSTANT(module, LEVEL_JRB) \
    LEVEL_CONSTANT(module, LEVEL_THI) \
    LEVEL_CONSTANT(module, LEVEL_TTC) \
    LEVEL_CONSTANT(module, LEVEL_RR) \
    LEVEL_CONSTANT(module, LEVEL_CASTLE_GROUNDS) \
    LEVEL_CONSTANT(module, LEVEL_BITDW) \
    LEVEL_CONSTANT(module, LEVEL_VCUTM) \
    LEVEL_CONSTANT(module, LEVEL_BITFS) \
    LEVEL_CONSTANT(module, LEVEL_SA) \
    LEVEL_CONSTANT(module, LEVEL_BITS) \
    LEVEL_CONSTANT(module, LEVEL_LLL) \
    LEVEL_CONSTANT(module, LEVEL_DDD) \
    LEVEL_CONSTANT(module, LEVEL_WF) \
    LEVEL_CONSTANT(module, LEVEL_ENDING) \
    LEVEL_CONSTANT(module, LEVEL_CASTLE_COURTYARD) \
    LEVEL_CONSTANT(module, LEVEL_PSS) \
    LEVEL_CONSTANT(module, LEVEL_COTMC) \
    LEVEL_CONSTANT(module, LEVEL_TOTWC) \
    LEVEL_CONSTANT(module, LEVEL_BOWSER_1) \
    LEVEL_CONSTANT(module, LEVEL_WMOTR) \
    LEVEL_CONSTANT(module, LEVEL_UNKNOWN_32) \
    LEVEL_CONSTANT(module, LEVEL_BOWSER_2) \
    LEVEL_CONSTANT(module, LEVEL_BOWSER_3) \
    LEVEL_CONSTANT(module, LEVEL_UNKNOWN_35) \
    LEVEL_CONSTANT(module, LEVEL_TTM) \
    LEVEL_CONSTANT(module, LEVEL_UNKNOWN_37) \
    LEVEL_CONSTANT(module, LEVEL_UNKNOWN_38)