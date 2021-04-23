cmake_minimum_required(VERSION 3.0.0)
project(sm64pc VERSION 0.1.0)

set(CMAKE_VERBOSE_MAKEFILE True)

# This is NOT YET FUNCTIONAL!

#include(CTest)
#enable_testing()

# Build debug version (default)
set(DEBUG 1)
# Version of the game to build
set(VERSION "us")
# Graphics microcode used
set(GRUCODE f3dex2e)
# If COMPARE is 1, check the output sha1sum when building 'all'
#set(COMPARE 1)
# If NON_MATCHING is 1, define the NON_MATCHING and AVOID_UB macros when building (recommended)
set(NON_MATCHING 1)
# Build and optimize for Raspberry Pi(s)
#set(TARGET_RPI ?= 0
# Build with python support.
set(USE_PYTHON 1)
# Version of python to use.
#set(PYTHON_VER := python3.6m
# Compile unit tests for python stuff (broken).
#CHECK_PYTHON ?= 0
# Build for Emscripten/WebGL
#TARGET_WEB ?= 0
# Makeflag to enable OSX fixes
#OSX_BUILD ?= 0
# Specify the target you are building for, TARGET_BITS=0 means native
#TARGET_ARCH ?= native
#TARGET_BITS ?= 0
# Disable better camera by default
set(BETTERCAMERA 0)
# Disable no drawing distance by default
set(NODRAWINGDISTANCE 0)
# Disable texture fixes by default (helps with them purists)
set(TEXTURE_FIX 0)
# Enable extended options menu by default
set(EXT_OPTIONS_MENU 1)
# Disable text-based save-files by default
set(TEXTSAVES 0)
# Load resources from external files
set(EXTERNAL_DATA 0)
# Enable Discord Rich Presence
set(DISCORDRPC 0)
# Various workarounds for weird toolchains
set(NO_BZERO_BCOPY 0)
set(NO_LDIV 0)
# Backend selection
# Renderers: GL, GL_LEGACY, D3D11, D3D12
set(RENDER_API GL)
# Window managers: SDL2, DXGI (forced if D3D11 or D3D12 in RENDER_API)
set(WINDOW_API SDL2)
# Audio backends: SDL2
set(AUDIO_API SDL2)
# Controller backends (can have multiple, space separated): SDL2
set(CONTROLLER_API SDL2)
# Misc settings for EXTERNAL_DATA
set(BASEDIR res)
set(BASEPACK base.zip)
# Automatic settings for PC port(s)

set(BUILD_TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
message(STATUS "Using ${BUILD_TMP_DIR} for intermediate build files.")

set(TARGET)
Set(INCLUDE_DIRS
    "${BUILD_TMP_DIR}/gfx"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VERSION_C_FLAGS "")

if("${VERSION}" MATCHES "us")
    set(VERSION_C_FLAGS "${VERSION_C_FLAGS} -DVERSION_US")
    set(VERSION_AS_FLAGS "--defsym VERSION_US=1")
    set(GRUCODE_CFLAGS "-DF3D_OLD")
    set(GRUCODE_ASFLAGS "--defsym F3D_OLD=1")
    set(TARGET "sm64.us")
endif()

if(WIN32 OR UNIX)
    set(VERSION_C_FLAGS "${VERSION_C_FLAGS} -DNON_MATCHING -DAVOID_UB")
endif()

if(NOT TARGET_BITS EQUAL 0)
  set(BITS "-m${TARGET_BITS}")
endif()

# TODO
# Stuff for showing the git hash in the intro on nightly builds
# From https://stackoverflow.com/questions/44038428/include-git-commit-hash-and-or-branch-name-in-c-c-source
#ifeq ($(shell git rev-parse --abbrev-ref HEAD),nightly)
#  GIT_HASH=`git rev-parse --short HEAD`
#  COMPILE_TIME=`date -u +'%Y-%m-%d %H:%M:%S UTC'`
#  VERSION_CFLAGS += -DNIGHTLY -DGIT_HASH="\"$(GIT_HASH)\"" -DCOMPILE_TIME="\"$(COMPILE_TIME)\""
#endif

if("${GRUCODE}" MATCHES "f3dex2e")
    set(GRUCODE_CFLAGS "-DF3DEX_GBI")
    set(GRUCODE_ASFLAGS "--defsym F3DEX_GBI_SHARED=1 --defsym F3DEX_GBI=1")
    set(TARGET "${TARGET}.f3dex")
    #COMPARE := 0
endif()

if("${USE_PYTHON}" EQUAL 1)
    set(Python_ADDITIONAL_VERSIONS 3.6)
    find_package(PythonLibs 3.6)
    set(VERSION_C_FLAGS "${VERSION_C_FLAGS} -DUSE_PYTHON")
    set(INCLUDE_DIRS "${INCLUDE_DIRS}" "${PYTHON_INCLUDE_DIRS}")
endif()

# Assets Extraction

find_package(PythonInterp 3)

# Make sure assets exist
set(NOEXTRACT 0)
if(NOEXTRACT EQUAL 0)
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/extract_assets.py" "${VERSION}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    #if(NOT EXTRACT_RESULT)
    #    message(FATAL_ERROR "Failed to extract assets.")
    #else()
    message(STATUS "Assets extracted.")
    #endif()
endif()

# TODO
# Make tools if out of date
#DUMMY != make -C tools >&2 || echo FAIL
#ifeq ($(DUMMY),FAIL)
#  $(error Failed to build tools)
#endif

# Sources

set(SRC_DIRS
    src/engine
    src/game
    src/audio
    src/menu
    src/buffers
    actors
    levels
    bin
    data
    assets
    src/pc
    src/pc/gfx
    src/pc/audio
    src/pc/controller
    src/pc/fs src/pc/fs/packtypes)

set(GODDARD_SRC_DIRS
    src/goddard
    src/goddard/dynlists)

set(ULTRA_SRC_DIRS lib/src lib/src/math)

file(GLOB LEVEL_C_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/levels/*/leveldata.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/levels/*/script.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/levels/*/geo.c")

set(C_FILES)
foreach(SRC_DIR ${SRC_DIRS})
    file(GLOB DIR_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.c")
    set(C_FILES ${C_FILES} ${DIR_C_FILES})
endforeach()

set(GODDARD_C_FILES)
foreach(SRC_DIR ${GODDARD_SRC_DIRS})
    file(GLOB DIR_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.c")
    set(GODDARD_C_FILES ${GODDARD_C_FILES} ${DIR_C_FILES})
endforeach()

set(CXX_FILES)
foreach(SRC_DIR ${SRC_DIRS})
    file(GLOB DIR_CXX_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.cpp")
    set(CXX_FILES ${CXX_FILES} ${DIR_CXX_FILES})
endforeach()

set(ULTRA_C_FILES)
foreach(SRC_DIR ${ULTRA_SRC_DIRS})
    file(GLOB DIR_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.c")
    set(ULTRA_C_FILES ${ULTRA_C_FILES} ${DIR_C_FILES})
endforeach()

#set(AsmSources)
#foreach(AsmDir ${AsmDirs})
#    file(GLOB CXXFiles "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.s")
#    set(AsmSources ${AsmSources} ${AsmFiles})
#endforeach()

#S_FILES := $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.s))
#ULTRA_C_FILES := $(foreach dir,$(ULTRA_SRC_DIRS),$(wildcard $(dir)/*.c))
#GODDARD_C_FILES := $(foreach dir,$(GODDARD_SRC_DIRS),$(wildcard $(dir)/*.c))

list(REMOVE_ITEM ULTRA_C_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/sqrtf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/sprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/_Printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/kdebugserver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osInitialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/func_802F7140.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/func_802F71F0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/func_802F4A20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/EU_D_802f4330.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/D_802F4380.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osLeoDiskInit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osCreateThread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osDestroyThread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osStartThread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSetThreadPri.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osPiStartDma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osPiRawStartDma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osPiRawReadIo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osPiGetCmdQueue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osJamMesg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSendMesg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osRecvMesg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSetEventMesg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osTimer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSetTimer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSetTime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osCreateViManager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osViSetSpecialFeatures.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osVirtualToPhysical.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osViBlack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osViSetEvent.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osViSetMode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osViSwapBuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osSpTaskLoadGo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osCreatePiManager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osGetTime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osEepromProbe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osEepromWrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osEepromLongWrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osEepromRead.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osEepromLongRead.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osContInit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osContStartReadData.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osAiGetLength.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osAiSetFrequency.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/osAiSetNextBuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osViInit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osSyncPutChars.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osAtomicDec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osSiRawStartDma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osViSwapContext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osViGetCurrentContext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/__osDevMgrMain)

list(REMOVE_ITEM C_FILES src/game/main.c)

# Build Directory Creation

set(BUILD_TMP_DIR_CHILDREN
    "${BUILD_TMP_DIR}/include"
    "${BUILD_TMP_DIR}/bin"
    "${BUILD_TMP_DIR}/gfx")
foreach(CHILD in ${BUILD_TMP_DIR_CHILDREN})
    message(STATUS "Creating subdirectory ${CHILD}...")
    file(MAKE_DIRECTORY "${CHILD}")
endforeach()

# Texture Generation

set(N64GRAPHICS "${CMAKE_CURRENT_SOURCE_DIR}/tools/n64graphics")

file(GLOB SKYBOX_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/textures/skyboxes/*.png")

foreach(SKYBOX ${SKYBOX_PNGS})
    get_filename_component(SKYBOX_BASENAME "${SKYBOX}" NAME_WE)
    set(SKYBOX_DEST "${BUILD_TMP_DIR}/bin/${SKYBOX_BASENAME}_skybox.o")
    #string(REPLACE "")
    #message(STATUS ${SKYBOX})
    if(EXTERNAL_DATA EQUAL 1)
        add_custom_target(
            "${SKYBOX_BASENAME}_skybox.o"
            COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/zeroterm.py" "${SKYBOX}"
            COMMENT "Preprocessing ${SKYBOX}"
        )
    else()
        add_custom_target(
            "${SKYBOX_BASENAME}_skybox.o"
            COMMAND "${N64GRAPHICS}" "-i" "${SKYBOX_DEST}" "-g" "${SKYBOX}"
            COMMENT "Preprocessing ${SKYBOX}"
        )
    endif()
endforeach()

file(GLOB_RECURSE ACTOR_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/actors/*.png")
file(GLOB_RECURSE LEVEL_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/levels/*.png")
file(GLOB_RECURSE TEXTURE_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/textures/*.png")

set(TEXTURE_PNGS ${ACTOR_TEXTURE_PNGS} ${LEVEL_TEXTURE_PNGS} ${TEXTURE_TEXTURE_PNGS})
list(REMOVE_DUPLICATES TEXTURE_PNGS)
list(REMOVE_ITEM TEXTURE_PNGS ${SKYBOX_PNGS})

set(TEXTURE_INC_C_FILES ${TEXTURE_PNGS})
string(REGEX REPLACE "[.]png" ".inc.c" TEXTURE_INC_C_FILES "${TEXTURE_PNG}")

set(TEXTURE_INC_C_DIRECTORIES "")

#add_custom_target("texture_pngs" BYPRODUCTS ${TEXTURE_INC_C_FILES})
foreach(TEXTURE_PNG ${TEXTURE_PNGS})
    string(REGEX REPLACE "[.]png$" "" TEXTURE_BASENAME_FULL "${TEXTURE_PNG}")
    get_filename_component(TEXTURE_DIRECTORY_FULL "${TEXTURE_PNG}" DIRECTORY)
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_DIRECTORY "${TEXTURE_DIRECTORY_FULL}")
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_BASENAME "${TEXTURE_BASENAME_FULL}")
    #message(STATUS "${TEXTURE_BASENAME}")
    set(TEXTURE_INC_C_DIRECTORIES ${TEXTURE_INC_C_DIRECTORIES} "${BUILD_TMP_DIR}/gfx${TEXTURE_DIRECTORY}")
    file(MAKE_DIRECTORY "${BUILD_TMP_DIR}/gfx${TEXTURE_DIRECTORY}")
    if(EXTERNAL_DATA EQUAL 1)
    else()
        #OUTPUT "${BUILD_TMP_DIR}/gfx/${TEXTURE_BASENAME}.inc.c"
        add_custom_command(
            TARGET "texture_pngs" PRE_BUILD
            COMMAND "hexdump" "-v" "-e" "1/1 \"0x%X,\"" "${TEXTURE_PNG}" > "${BUILD_TMP_DIR}/gfx${TEXTURE_BASENAME}.inc.c"
            COMMENT "Preprocessing ${TEXTURE_BASENAME}.inc.c"
            VERBATIM
        )
    endif()
endforeach()
list(REMOVE_DUPLICATES TEXTURE_INC_C_DIRECTORIES)

# Levels Header

add_custom_command(
    OUTPUT "${BUILD_TMP_DIR}/include/level_headers.h"
    COMMAND "${CMAKE_C_COMPILER}" -I "${CMAKE_CURRENT_SOURCE_DIR}" -E "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in" | "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/output_level_headers.py" > "${BUILD_TMP_DIR}/include/level_headers.h"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in"
    COMMENT "Preprocessing level_headers.h"
    VERBATIM)

# Localized Headers

set(LOCALIZE_HEADERS 
"text_strings.h"
"text_menu_strings.h"
"text_options_strings.h")

foreach(HEADER ${LOCALIZE_HEADERS})
add_custom_target(
    ${HEADER}
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/textconv" "${CMAKE_CURRENT_SOURCE_DIR}/charmap.txt" "${CMAKE_CURRENT_SOURCE_DIR}/include/${HEADER}.in" "${BUILD_TMP_DIR}/include/${HEADER}"
    BYPRODUCTS "${BUILD_TMP_DIR}/include/${HEADER}"
    RESULT_VARIABLE TEXTCONV_RESULT)
#if(NOT TEXTCONV_RESULT)
#    message(FATAL_ERROR "Failed to convert header ${HEADER}.")
#else()
#    message(STATUS "Header ${HEADER} converted.")
#endif()
endforeach()

#GENERATED_C_FILES := $(BUILD_DIR)/assets/mario_anim_data.c $(BUILD_DIR)/assets/demo_data.c \
#  $(addprefix $(BUILD_DIR)/bin/,$(addsuffix _skybox.c,$(notdir $(basename $(wildcard textures/skyboxes/*.png)))))


# RGBA32, RGBA16, IA16, IA8, IA4, IA1, I8, I4
#if(EXTERNAL_DATA 1)
#$(BUILD_DIR)/%: %.png
#	$(ZEROTERM) "$(patsubst %.png,%,$^)" > $@
#else()
#$(BUILD_DIR)/%: %.png
#	$(N64GRAPHICS) -i $@ -g $< -f $(lastword $(subst ., ,$@))
#endif()

#TODO: ifeq ($(EXTERNAL_DATA),1)

Set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${VERSION_C_FLAGS}")

add_executable("${TARGET}" "${C_FILES};${CXX_FILES};${LEVEL_C_FILES}")

add_dependencies("${TARGET}" "texture_pngs")

# Add localized text headers.
foreach(HEADER ${LOCALIZE_HEADERS})
    add_dependencies("${TARGET}" ${HEADER})
endforeach()
target_include_directories("${TARGET}" PUBLIC "${BUILD_TMP_DIR}/include")
foreach(INCLUDE_DIR in ${INCLUDE_DIRS})
    target_include_directories("${TARGET}" PUBLIC "${INCLUDE_DIR}")
endforeach()
#foreach(INCLUDE_DIR in ${TEXTURE_INC_C_DIRECTORIES})
#    target_include_directories("${TARGET}" PUBLIC "${INCLUDE_DIR}")
#endforeach()

foreach(SKYBOX ${SKYBOX_PNGS})
    get_filename_component(SKYBOX_BASENAME "${SKYBOX}" NAME_WE)
    add_dependencies("${TARGET}" "${SKYBOX_BASENAME}_skybox.o")
endforeach()
