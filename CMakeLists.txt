
cmake_minimum_required(VERSION 3.0.0)
project(sm64pc VERSION 0.1.0)
enable_language(C ASM)

if(NOT DEFINED QUIET_CMAKE)
    set(QUIET_CMAKE True)
endif()

if(NOT DEFINED REALLY_QUIET_CMAKE)
    set(REALLY_QUIET_CMAKE False)
endif()

if(REALLY_QUIET_CMAKE)
    function(message)
        list(GET ARGV 0 MessageType)
        if(MessageType STREQUAL FATAL_ERROR OR
            MessageType STREQUAL SEND_ERROR OR
            MessageType STREQUAL WARNING OR
            MessageType STREQUAL AUTHOR_WARNING)
            list(REMOVE_AT ARGV 0)
            _message(${MessageType} "${ARGV}")
        endif()
    endfunction()
endif()

if(NOT QUIET_CMAKE)
    set(CMAKE_VERBOSE_MAKEFILE True)
endif()

add_subdirectory("tools")

# Build debug version (default)
#set(DEBUG 1)
# Version of the game to build
set(VERSION "us")

# Graphics microcode used
if(NOT DEFINED GRUCODE)
    set(GRUCODE "f3dex2e")
endif()

# If COMPARE is 1, check the output sha1sum when building 'all'
#set(COMPARE 1)

# If NON_MATCHING is 1, define the NON_MATCHING and AVOID_UB macros when building (recommended)
set(NON_MATCHING True)

# Build and optimize for Raspberry Pi(s)
#set(TARGET_RPI ?= 0

# Build with python support.
if(NOT DEFINED USE_PYTHON)
    set(USE_PYTHON True)
endif()

# Build for Emscripten/WebGL
#TARGET_WEB ?= 0
# Makeflag to enable OSX fixes
#OSX_BUILD ?= 0
# Specify the target you are building for, TARGET_BITS=0 means native
#TARGET_ARCH ?= native
#TARGET_BITS ?= 0

# Disable better camera by default
if(NOT DEFINED BETTER_CAMERA)
    set(BETTER_CAMERA False)
endif()

# Disable no drawing distance by default
if(NOT DEFINED NO_DRAWING_DISTANCE)
    set(NO_DRAWING_DISTANCE False)
endif()

# Disable texture fixes by default (helps with them purists)
if(NOT DEFINED TEXTURE_FIX)
    set(TEXTURE_FIX False)
endif()

# Enable extended options menu by default
if(NOT DEFINED EXT_OPTIONS_MENU)
    set(EXT_OPTIONS_MENU True)
endif()

# Disable text-based save-files by default
if(NOT DEFINED TEXT_SAVES)
    set(TEXT_SAVES False)
endif()

# Load resources from external files
if(NOT DEFINED EXTERNAL_DATA)
    set(EXTERNAL_DATA False)
endif()

if(NOT DEFINED CONFIGFILE_DEFAULT)
    set(CONFIGFILE_DEFAULT "sm64config.txt")
endif()

# Enable Discord Rich Presence
#set(DISCORDRPC False)

# Various workarounds for weird toolchains
#set(NO_BZERO_BCOPY 0)
#set(NO_LDIV 0)

# -= Backend selection =-

# Renderers: GL, GL_LEGACY, D3D11, D3D12
if(NOT DEFINED RENDER_API)
    set(RENDER_API GL)
endif()

# Window managers: SDL2, DXGI (forced if D3D11 or D3D12 in RENDER_API)
if(NOT DEFINED WINDOW_API)
    set(WINDOW_API SDL2)
endif()

# Audio backends: SDL2
if(NOT DEFINED AUDIO_API)
    set(AUDIO_API SDL2)
endif()

# Controller backends (can have multiple, space separated): SDL2
if(NOT DEFINED CONTROLLER_API)
    set(CONTROLLER_API SDL2)
endif()

if(NOT DEFINED FORCE_STANDALONE)
    set(FORCE_STANDALONE False)
endif()

# INSTALL_*_ROOT will be used relative to the CMAKE_INSTALL_PREFIX by the installer.
if(NOT DEFINED INSTALL_BIN_ROOT)
    if(WIN32 OR MINGW OR FORCE_STANDALONE)
        set(INSTALL_BIN_ROOT "/")
    elseif(UNIX)
        set(INSTALL_BIN_ROOT "/bin/")
    endif()
endif()

if(NOT DEFINED INSTALL_RES_ROOT)
    if(WIN32 OR MINGW OR FORCE_STANDALONE)
        set(INSTALL_RES_ROOT "/")
    elseif(UNIX)
        set(INSTALL_RES_ROOT "/share/sm64/")
    endif()
endif()

if(NOT DEFINED INSTALL_SCRIPT_ROOT)
    if(WIN32 OR MINGW OR FORCE_STANDALONE)
        set(INSTALL_SCRIPT_ROOT "/")
    else()
        set(INSTALL_SCRIPT_ROOT "/share/sm64/")
    endif()
endif()

# Misc settings for EXTERNAL_DATA
if(NOT DEFINED EXT_BASE_DIR)
    if(WIN32 OR MINGW)
        set(EXT_BASE_DIR "res")
    elseif(UNIX)
        set(EXT_BASE_DIR "${CMAKE_INSTALL_PREFIX}${INSTALL_RES_ROOT}")
    endif()
endif()

if(NOT DEFINED EXT_SOUND_DIR)
    set(EXT_SOUND_DIR "sound")
endif()

if(NOT DEFINED EXT_TEXTURE_DIR)
    set(EXT_TEXTURE_DIR "gfx")
endif()

if(NOT DEFINED SCRIPT_DIR)
    set(SCRIPT_DIR "${EXT_BASE_DIR}")
endif()

if(NOT DEFINED BASEPACK)
    set(BASEPACK base.zip)
endif()

if(NOT DEFINED BUILD_TEXTURES)
    set(BUILD_TEXTURES True)
endif()

# -= Process Configuration =-

message(STATUS "Installing binaries to ${CMAKE_INSTALL_PREFIX}${INSTALL_BIN_ROOT}")
message(STATUS "Installing resources to ${CMAKE_INSTALL_PREFIX}${INSTALL_RES_ROOT}")
if(USE_PYTHON)
    message(STATUS "Installing python script to ${SCRIPT_DIR}")
endif()

set(BUILD_TMP_DIR "${CMAKE_BINARY_DIR}/tmp")
if(MSVC)
   set(TOOLS_BIN_DIR "${CMAKE_BINARY_DIR}/tools/Debug")
else()
   set(TOOLS_BIN_DIR "${CMAKE_BINARY_DIR}/tools")
endif()
message(STATUS "Using ${BUILD_TMP_DIR} for intermediate build files.")

set(TARGET)
Set(INCLUDE_DIRS
    "${BUILD_TMP_DIR}"
    "${BUILD_TMP_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(VERSION_CFLAGS)
set(MISC_CFLAGS)

list(APPEND MISC_CFLAGS "-DCONFIGFILE_DEFAULT=\"\\\"${CONFIGFILE_DEFAULT}\\\"\"")

if(FORCE_STANDALONE)
    message(STATUS "Using Force Standalone")
else()
    message(STATUS "NOT using Force Standalone")
endif()

# This gets clobbered below, but that's how it is in the original Makefile...
if(VERSION MATCHES "us")
    list(APPEND VERSION_CFLAGS "-DVERSION_US")
    #set(VERSION_AS_FLAGS "--defsym VERSION_US=1")
    set(GRUCODE_CFLAGS "-DF3D_OLD")
    set(GRUCODE_ASFLAGS "--defsym F3D_OLD=1")
    set(TARGET "sm64.us")
endif()

if(USE_PYTHON)
    list(APPEND VERSION_CFLAGS
        "-DUSE_PYTHON"
        "-DPYTHON_SCRIPT_PATH=\"\\\"\${SCRIPT_DIR}\\\"\"")
    message(STATUS "Using Python.")
else()
    message(STATUS "NOT using Python.")
endif()

if(BETTER_CAMERA)
    list(APPEND MISC_CFLAGS "-DBETTERCAMERA")
    set(EXT_OPTIONS_MENU True)
    message(STATUS "Using Better Camera.")
else()
    message(STATUS "NOT using Better Camera.")
endif()

if(TEXT_SAVES)
    list(APPEND MISC_CFLAGS "-DTEXTSAVES")
    message(STATUS "Using Text Saves.")
else()
message(STATUS "NOT using Text Saves.")
endif()

if(NO_DRAWING_DISTANCE)
    list(APPEND MISC_CFLAGS "-DNODRAWINGDISTANCE")
else()
    message(STATUS "NOT using No Drawing Distance.")
endif()

if(WIN32 OR UNIX OR MINGW)
    list(APPEND VERSION_CFLAGS "-DAVOID_UB")
    set(NON_MATCHING True)
endif()

if(NON_MATCHING)
    list(APPEND VERSION_CFLAGS "-DNON_MATCHING")
endif()

if(RENDER_API MATCHES "D3D11" OR RENDER_API MATCHES "D3D12")
    if(NOT WINDOW_API MATCHES "DXGI")
        message(WARNING "DirectX renderers require DXGI; forcing WINDOW_API value.")
        set(WINDOW_API DXGI)
    endif()
else()
    if(WINDOW_API MATCHES "DXGI")
        message(error "DXGI can only be used with DirectX renderers.")
    endif()
endif()

if(WINDOW_API MATCHES "SDL2")
    list(APPEND BACKEND_CFLAGS "-DHAVE_SDL2=1")
endif()

# Texture Fix
if(TEXTURE_FIX)
    list(APPEND MISC_CFLAGS "-DTEXTURE_FIX")
    message(STATUS "Using Texture Fix.")
else()
    message(STATUS "NOT using Texture Fix.")
endif()

# Extended Options Menu
if(EXT_OPTIONS_MENU)
    list(APPEND MISC_CFLAGS "-DEXT_OPTIONS_MENU")
    message(STATUS "Using Extended Options Menu.")
else()
    message(STATUS "NOT using Extended Options Menu.")
endif()

if(EXTERNAL_DATA)
    message(STATUS "Using External Data.")
else()
    message(STATUS "NOT using External Data.")
endif()

# Load External Textures
set(SKYCONV_ARGS)
if(EXTERNAL_DATA)
    list(APPEND MISC_CFLAGS
        "-DEXTERNAL_DATA"
        "-DFS_BASEDIR=\"\\\"${EXT_BASE_DIR}\\\"\""
        "-DFS_TEXTUREDIR=\"\\\"${EXT_TEXTURE_DIR}\\\"\""
        "-DFS_SOUNDDIR=\"\\\"${EXT_SOUND_DIR}\\\"\"")
    
    # Tell skyconv to write names instead of actual texture data
    # and save the split tiles so we can use them later.
    set(SKYTILE_DIR "${BUILD_TMP_DIR}/${EXT_TEXTURE_DIR}/textures/skybox_tiles")
    
    add_custom_command(
        OUTPUT "${SKYTILE_DIR}"
        COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${SKYTILE_DIR}" 
    )

    set(SKYCONV_ARGS  "--store-names" "--write-tiles" "${SKYTILE_DIR}")
endif()

#if(NOT ${TARGET_BITS} EQUAL 0)
#  set(BITS "-m${TARGET_BITS}")
#endif()

# TODO: Figure out how to make this smarter?
set(CMAKE_ASM_COMPILER "as")

set(VERSION_ASFLAGS "--defsym AVOID_UB=1")
set(CMAKE_ASFLAGS "${CMAKE_ASFLAGS} -I ${CMAKE_CURRENT_SOURCE_DIR}/include -I ${BUILD_TMP_DIR} ${VERSION_ASFLAGS}")
separate_arguments(CMAKE_ASFLAGS)

# TODO
# Stuff for showing the git hash in the intro on nightly builds
# From https://stackoverflow.com/questions/44038428/include-git-commit-hash-and-or-branch-name-in-c-c-source
#ifeq ($(shell git rev-parse --abbrev-ref HEAD),nightly)
#  GIT_HASH=`git rev-parse --short HEAD`
#  COMPILE_TIME=`date -u +'%Y-%m-%d %H:%M:%S UTC'`
#  VERSION_CFLAGS += -DNIGHTLY -DGIT_HASH="\"$(GIT_HASH)\"" -DCOMPILE_TIME="\"$(COMPILE_TIME)\""
#endif

# This clobbers the CFLAGS from the version-based check above.
if(GRUCODE MATCHES "f3dex") # Fast3DEX
    set(GRUCODE_CFLAGS "-DF3DEX_GBI")
    #set(GRUCODE_ASFLAGS "--defsym F3DEX_GBI_SHARED=1" "--defsym F3DEX_GBI=1")
    set(TARGET "${TARGET}.f3dex")

elseif(GRUCODE MATCHES "f3dex2") # Fast3DEX2
    set(GRUCODE_CFLAGS "-DF3DEX_GBI_2")
    #set(GRUCODE_ASFLAGS "--defsym F3DEX_GBI_SHARED=1" "--defsym F3DEX_GBI_2=1")
    set(TARGET "${TARGET}.f3dex2")

elseif(GRUCODE MATCHES "f3dex2e") # Fast3DEX2 Extended (PC default)
    set(GRUCODE_CFLAGS "-DF3DEX_GBI_2E")
    set(TARGET "${TARGET}.f3dex2e")

elseif(GRUCODE MATCHES "f3d_new") # Fast3D 2.0H (Shindou)
    set(GRUCODE_CFLAGS "-DF3D_NEW")
    #set(GRUCODE_ASFLAGS "--defsym F3D_NEW=1")
    set(TARGET "${TARGET}.f3d_new")

elseif(GRUCODE MATCHES "f3dzex") # Fast3DZEX (2.0J / Animal Forest - Dōbutsu no Mori)
    message(warning "Fast3DZEX is experimental. Try at your own risk.")
    set(GRUCODE_CFLAGS "-DF3DEX_GBI_2")
    #set(GRUCODE_ASFLAGS "--defsym F3DEX_GBI_SHARED=1" "--defsym F3DZEX_GBI=1")
    set(TARGET "${TARGET}.f3dzex")
endif()

message(STATUS "Using ${GRUCODE} microcode.")

set(SOUND_BIN_REL_DIR "sound")
set(SOUND_BIN_DIR "${BUILD_TMP_DIR}/${SOUND_BIN_REL_DIR}")
set(SEQUENCES_BIN_DIR "${BUILD_TMP_DIR}/${SOUND_BIN_REL_DIR}/sequences")
set(SOUND_DATA_CTL_REL_PATH "${SOUND_BIN_REL_DIR}/sound_data.ctl")
set(SOUND_DATA_CTL_PATH "${BUILD_TMP_DIR}/${SOUND_DATA_CTL_REL_PATH}")
set(SOUND_DATA_TBL_REL_PATH "${SOUND_BIN_REL_DIR}/sound_data.tbl")
set(SOUND_DATA_TBL_PATH "${BUILD_TMP_DIR}/${SOUND_DATA_TBL_REL_PATH}")
set(SEQUENCES_BIN_REL_PATH "${SOUND_BIN_REL_DIR}/sequences.bin")
set(SEQUENCES_BIN_PATH "${BUILD_TMP_DIR}/${SEQUENCES_BIN_REL_PATH}")
set(BANK_SETS_REL_PATH "${SOUND_BIN_REL_DIR}/bank_sets")
set(BANK_SETS_PATH "${BUILD_TMP_DIR}/${BANK_SETS_REL_PATH}")
set(ASSETS_BIN_DIR "${BUILD_TMP_DIR}/assets")
set(MARIO_ANIM_DATA_C_PATH "${ASSETS_BIN_DIR}/mario_anim_data.c")
set(DEMO_DATA_C_PATH "${ASSETS_BIN_DIR}/demo_data.c")
set(CHARMAP_TXT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/charmap.txt")

if(MSVC)
   set(CPP_COMMAND "cl")
else()
   set(CPP_COMMAND "cpp")
endif()
set(CPP_ARGS "-P")
set(TEXTCONV_COMMAND "${TOOLS_BIN_DIR}/textconv")
set(TEXTCONV_TARGET "textconv")
set(N64GRAPHICS_COMMAND "${TOOLS_BIN_DIR}/n64graphics")
set(N64GRAPHICS_TARGET "n64graphics")
set(N64GRAPHICS_CI_COMMAND "${TOOLS_BIN_DIR}/n64graphics_ci")
set(N64GRAPHICS_CI_TARGET "n64graphics_ci")
set(SKYCONV_COMMAND "${TOOLS_BIN_DIR}/skyconv")
set(SKYCONV_TARGET "skyconv")
set(AIFF_EXTRACT_CODEBOOK_COMMAND "${TOOLS_BIN_DIR}/aiff_extract_codebook")
set(AIFF_EXTRACT_CODEBOOK_TARGET "aiff_extract_codebook")
set(VADPCM_ENC_COMMAND "${TOOLS_BIN_DIR}/vadpcm_enc")
set(VADPCM_ENC_TARGET "vadpcm_enc")
set(MKHEADER_COMMAND "${TOOLS_BIN_DIR}/mkheader")
set(MKHEADER_TARGET "mkheader")
set(ZEROTERM_PY "${CMAKE_CURRENT_SOURCE_DIR}/tools/zeroterm.py")
set(MARIO_ANIMS_CONVERTER_PY "${CMAKE_CURRENT_SOURCE_DIR}/tools/mario_anims_converter.py")
set(DEMO_DATA_CONVERTER_PY "${CMAKE_CURRENT_SOURCE_DIR}/tools/demo_data_converter.py")
set(OUTPUT_LEVEL_HEADERS_PY "${CMAKE_CURRENT_SOURCE_DIR}/tools/output_level_headers.py")

set(DATA_O_FILES)

set(DATA_C_FILES
    "${SOUND_DATA_CTL_PATH}.c"
    "${SOUND_DATA_TBL_PATH}.c"
    "${SEQUENCES_BIN_PATH}.c"
    "${BANK_SETS_PATH}.c")

macro(hexdump_binary_assets bin_file c_file array_name array_sz)
    add_custom_command(
        OUTPUT "${c_file}"
        COMMAND 
            "${MKHEADER_COMMAND}" "${bin_file}" "${c_file}" "${array_name}" "${array_sz}"
        DEPENDS "${bin_file}" "${MKHEADER_TARGET}"
        COMMENT "Generating ${c_file} from ${bin_file}"
        VERBATIM
    )
endmacro()

macro(make_directory_target path)
add_custom_command(
    OUTPUT "${path}"
    COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${path}"
    COMMENT "Creating directory ${path}"
    VERBATIM
)
endmacro()

# -= Endian and Bitwidth Tests =-

execute_process(
    COMMAND "${CMAKE_C_COMPILER}" "-c" ${CMAKE_C_FLAGS} "-I" "${CMAKE_CURRENT_SOURCE_DIR}/include" "-o" "${ENDIAN_BITWIDTH}.dummy2" "${CMAKE_CURRENT_SOURCE_DIR}/tools/determine-endian-bitwidth.c"
    ERROR_VARIABLE BITWIDTH_ENDIAN_ARGS
    ERROR_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE ".*msgbegin[ ]*(.*)[ ]*msgend.*" "\\1" BITWIDTH_ENDIAN_ARGS "${BITWIDTH_ENDIAN_ARGS}")
separate_arguments(BITWIDTH_ENDIAN_ARGS)

# -= Backend CFLAGS =-

set(BACKEND_CFLAGS
    "-DRAPI_${RENDER_API}=1"
    "-DWAPI_${WINDOW_API}=1"
    "-DAAPI_${AUDIO_API}=1")

# Can have multiple controller APIs.
foreach(CAPI ${CONTROLLER_API})
    list(APPEND BACKEND_CFLAGS "-DCAPI_${CAPI}=1")
endforeach()

# -= Sources =-

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
    list(APPEND C_FILES ${DIR_C_FILES})
endforeach()
list(REMOVE_DUPLICATES C_FILES)
list(REMOVE_ITEM C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/game/main.c")

set(GODDARD_C_FILES)
foreach(SRC_DIR ${GODDARD_SRC_DIRS})
    file(GLOB DIR_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.c")
    list(APPEND GODDARD_C_FILES ${DIR_C_FILES})
endforeach()

set(CXX_FILES)
foreach(SRC_DIR ${SRC_DIRS})
    file(GLOB DIR_CXX_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.cpp")
    list(APPEND CXX_FILES ${DIR_CXX_FILES})
endforeach()

set(ULTRA_C_FILES)
foreach(SRC_DIR ${ULTRA_SRC_DIRS})
    file(GLOB DIR_C_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/*.c")
    list(APPEND ULTRA_C_FILES ${DIR_C_FILES})
endforeach()

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

# -= Assets Extraction =-

file(GLOB SOUND_BANK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sound/sound_banks/*.json")

if(VERSION MATCHES "sh")
#    file(GLOB SOUND_SEQUENCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/jp/*.s"
#        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/*.s"
#        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/jp/*.m64"
#        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/*.m64")
else()
    file(GLOB SOUND_SEQUENCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/${VERSION}/*.m64"
        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/*.m64")
    file(GLOB SOUND_SEQUENCE_S_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/${VERSION}/*.s"
        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences/*.s")
endif()

file(GLOB SOUND_SAMPLE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/sound/samples/*")
set(SOUND_SAMPLE_AIFCS)
set(SOUND_SAMPLE_TABLES)
foreach(SAMPLE_DIR ${SOUND_SAMPLE_DIRS})

    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" SAMPLE_BUILD_DIR "${SAMPLE_DIR}")

    make_directory_target("${SAMPLE_BUILD_DIR}")

    file(GLOB DIR_AIFFS "${SAMPLE_DIR}/*.aiff")

    foreach(AIFF ${DIR_AIFFS})
        string(REGEX REPLACE "[.]aiff$" ".aifc" AIFC "${AIFF}")
        string(REGEX REPLACE "[.]aiff$" ".table" TABLE "${AIFF}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" AIFC "${AIFC}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" TABLE "${TABLE}")

        list(APPEND SOUND_SAMPLE_AIFCS ${AIFC})
        
        add_custom_command(
            OUTPUT "${TABLE}"
            COMMAND "${AIFF_EXTRACT_CODEBOOK_COMMAND}" "${AIFF}" > "${TABLE}"
            DEPENDS "${AIFF_EXTRACT_CODEBOOK_TARGET}" "${AIFF}" "${SAMPLE_BUILD_DIR}"
            COMMENT "Encoding ${AIFF}"
            VERBATIM
        )
    
        add_custom_command(
            OUTPUT "${AIFC}"
            COMMAND "${VADPCM_ENC_COMMAND}" -c "${TABLE}" "${AIFF}" "${AIFC}"
            DEPENDS "${VADPCM_ENC_TARGET}" "${AIFF}" "${TABLE}"
            COMMENT "Encoding ${AIFF}"
            VERBATIM
        )
    endforeach()
endforeach()

find_package(PythonInterp 3)

# Make sure assets exist
execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/extract_assets.py" "${VERSION}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Assets extracted.")

add_custom_command(
    OUTPUT "${SOUND_DATA_CTL_PATH}" "${SOUND_DATA_TBL_PATH}"
    COMMAND "${PYTHON_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/assemble_sound.py"
        "${BUILD_TMP_DIR}/sound/samples/"
        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sound_banks/"
        "${SOUND_DATA_CTL_PATH}"
        "${SOUND_DATA_TBL_PATH}"
        ${VERSION_CFLAGS}
        ${BITWIDTH_ENDIAN_ARGS}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS ${SOUND_BANK_FILES} ${SOUND_SAMPLE_AIFCS}
    COMMENT "Preprocessing sound_data.ctl"
    VERBATIM
)

foreach(S_FILE ${SOUND_SEQUENCE_S_FILES})
    string(REGEX REPLACE "[.]s$" ".m64" S_M64_FILE "${S_FILE}")
    string(REGEX REPLACE "[.]s$" ".o" S_O_FILE "${S_FILE}")
    string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" S_M64_FILE "${S_M64_FILE}")
    get_filename_component(S_M64_DIR "${S_M64_FILE}" DIRECTORY)
    list(APPEND SOUND_SEQUENCE_FILES "${S_M64_FILE}")
    make_directory_target("${S_M64_DIR}")
    add_custom_command(
        OUTPUT "${S_O_FILE}"
        COMMAND "${CMAKE_ASM_COMPILER}" ${CMAKE_ASFLAGS} "-o" "${S_O_FILE}" "${S_FILE}"
        DEPENDS "${S_FILE}" "${S_M64_DIR}"
        COMMENT "Assembling ${S_FILE}"
        VERBATIM
    )
    add_custom_command(
        OUTPUT "${S_M64_FILE}"
        COMMAND "${CMAKE_OBJCOPY}" "-j" ".rodata" "${S_O_FILE}" "-O" "binary" "${S_M64_FILE}"
        DEPENDS "${S_O_FILE}" 
        COMMENT "Translating ${S_O_FILE}"
        VERBATIM
    )
endforeach()

if(VERSION MATCHES "sh")
# $(SOUND_BIN_DIR)/sequences.bin: $(SOUND_BANK_FILES) sound/sequences.json sound/sequences/ sound/sequences/jp/ $(SOUND_SEQUENCE_FILES) $(ENDIAN_BITWIDTH)
# 	$(PYTHON) tools/assemble_sound.py --sequences $@ $(SOUND_BIN_DIR)/bank_sets sound/sound_banks/ sound/sequences.json $(SOUND_SEQUENCE_FILES) $(VERSION_CFLAGS) $$(cat $(ENDIAN_BITWIDTH))
else()
add_custom_command(
    OUTPUT "${SEQUENCES_BIN_PATH}" "${BANK_SETS_PATH}"
    COMMAND "${PYTHON_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/assemble_sound.py"
        "--sequences"
        "${SEQUENCES_BIN_PATH}"
        "${BANK_SETS_PATH}"
        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sound_banks/"
        "${CMAKE_CURRENT_SOURCE_DIR}/sound/sequences.json"
        ${SOUND_SEQUENCE_FILES}
        ${VERSION_CFLAGS}
        ${BITWIDTH_ENDIAN_ARGS}
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/assemble_sound.py"
        "${SOUND_BIN_DIR}"
        "${SEQUENCES_BIN_DIR}"
        ${SOUND_SEQUENCE_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Preprocessing sound_data.ctl"
    VERBATIM
)
endif()

if(EXTERNAL_DATA)

    add_custom_command(
        OUTPUT "${SOUND_DATA_CTL_PATH}.c"
        COMMAND "${MKHEADER_COMMAND}" "-e"
            "${SOUND_DATA_CTL_REL_PATH}"
            "${SOUND_DATA_CTL_PATH}.c"
            "gSoundDataADSR"
        DEPENDS "${SOUND_DATA_CTL_PATH}" "${MKHEADER_TARGET}" "${SOUND_BIN_DIR}"
        COMMENT "Generating ${SOUND_DATA_CTL_PATH}.c from ${SOUND_DATA_CTL_PATH}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${SOUND_DATA_TBL_PATH}.c"
        COMMAND "${MKHEADER_COMMAND}" "-e"
            "${SOUND_DATA_TBL_REL_PATH}"
            "${SOUND_DATA_TBL_PATH}.c"
            "gSoundDataRaw"
        DEPENDS "${SOUND_DATA_TBL_PATH}" "${MKHEADER_TARGET}" "${SOUND_BIN_DIR}"
        COMMENT "Generating ${SOUND_DATA_TBL_PATH}.c from ${SOUND_DATA_TBL_PATH}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${SEQUENCES_BIN_PATH}.c"
        COMMAND "${MKHEADER_COMMAND}" "-e"
            "${SEQUENCES_BIN_REL_PATH}"
            "${SEQUENCES_BIN_PATH}.c"
            "gMusicData"
        DEPENDS "${SEQUENCES_BIN_PATH}" "${MKHEADER_TARGET}" "${SOUND_BIN_DIR}"
        COMMENT "Generating ${SEQUENCES_BIN_PATH}.c from ${SEQUENCES_BIN_PATH}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${BANK_SETS_PATH}.c"
        COMMAND "${MKHEADER_COMMAND}" "-e"
            "${BANK_SETS_REL_PATH}"
            "${BANK_SETS_PATH}.c"
            "gBankSetsData"
        DEPENDS "${BANK_SETS_PATH}" "${MKHEADER_TARGET}" "${SOUND_BIN_DIR}"
        COMMENT "Generating ${BANK_SETS_PATH}.c from ${BANK_SETS_PATH}"
        VERBATIM
    )

else()

    hexdump_binary_assets(
        "${SOUND_DATA_CTL_PATH}"
        "${SOUND_DATA_CTL_PATH}.c"
        "gSoundDataADSR" ""
    )

    hexdump_binary_assets(
        "${SOUND_DATA_TBL_PATH}"
        "${SOUND_DATA_TBL_PATH}.c"
        "gSoundDataRaw" ""
    )

    hexdump_binary_assets(
        "${SEQUENCES_BIN_PATH}"
        "${SEQUENCES_BIN_PATH}.c"
        "gMusicData" ""
    )

    hexdump_binary_assets(
        "${BANK_SETS_PATH}"
        "${BANK_SETS_PATH}.c"
        "gBankSetsData" "0x100"
    )

endif()

make_directory_target("${ASSETS_BIN_DIR}")

file(GLOB MARIO_ANIM_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/anims/*.inc.c")
add_custom_command(
    OUTPUT "${MARIO_ANIM_DATA_C_PATH}"
    COMMAND "${PYTHON_EXECUTABLE}"
        "${MARIO_ANIMS_CONVERTER_PY}" >
        "${MARIO_ANIM_DATA_C_PATH}"
    DEPENDS ${MARIO_ANIM_DATA_FILES} "${MARIO_ANIMS_CONVERTER_PY}" "${ASSETS_BIN_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Converting animations"
    VERBATIM
)
list(APPEND DATA_C_FILES "${MARIO_ANIM_DATA_C_PATH}")

file(GLOB MARIO_DEMO_DATA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/demos/*.bin" )
add_custom_command(
    OUTPUT "${DEMO_DATA_C_PATH}"
    COMMAND "${PYTHON_EXECUTABLE}"
        "${DEMO_DATA_CONVERTER_PY}"
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/demo_data.json"
        ${VERSION_CFLAGS} >
        "${DEMO_DATA_C_PATH}"
    DEPENDS
        ${MARIO_DEMO_DATA_FILES}
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/demo_data.json"
        "${DEMO_DATA_CONVERTER_PY}"
        "${ASSETS_BIN_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Converting demo data"
    VERBATIM
)
list(APPEND DATA_C_FILES "${DEMO_DATA_C_PATH}")

# -= Texture Generation =-

make_directory_target("${BUILD_TMP_DIR}/bin")

file(GLOB SKYBOX_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/textures/skyboxes/*.png")
set(SKYBOX_C_FILES)
foreach(SKYBOX_PNG_PATH ${SKYBOX_PNGS})
    get_filename_component(SKYBOX_BASENAME "${SKYBOX_PNG_PATH}" NAME_WE)
    set(SKYBOX_C_PATH "${BUILD_TMP_DIR}/bin/${SKYBOX_BASENAME}_skybox.c")

    set(SKYBOX_DEPENDS
        "${SKYBOX_PNG_PATH}"
        "${BUILD_TMP_DIR}/bin"
        "${SKYCONV_TARGET}")
    
    if(EXTERNAL_DATA)
        list(APPEND SKYBOX_DEPENDS "${SKYTILE_DIR}")
    endif()

    add_custom_command(
        OUTPUT "${SKYBOX_C_PATH}"
        COMMAND "${SKYCONV_COMMAND}" ${SKYCONV_ARGS} "--type" "sky" "--split"
            "${SKYBOX_PNG_PATH}"
            "${BUILD_TMP_DIR}/bin"
        DEPENDS ${SKYBOX_DEPENDS}
        COMMENT "Preprocessing ${SKYBOX_PNG_PATH}"
        VERBATIM
    )

    # These are each their own translation unit, and must be compiled.
    list(APPEND DATA_C_FILES "${SKYBOX_C_PATH}")
endforeach()

# Gather the rest of the textures, remove skyboxes.
file(GLOB_RECURSE ACTOR_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/actors/*.png")
file(GLOB_RECURSE LEVEL_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/levels/*.png")
file(GLOB_RECURSE TEXTURE_TEXTURE_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/textures/*.png")

set(TEXTURE_PNGS ${ACTOR_TEXTURE_PNGS} ${LEVEL_TEXTURE_PNGS} ${TEXTURE_TEXTURE_PNGS})
list(REMOVE_DUPLICATES TEXTURE_PNGS)
list(REMOVE_ITEM TEXTURE_PNGS ${SKYBOX_PNGS}
    "${CMAKE_CURRENT_SOURCE_DIR}/levels/ending/cake.png")

string(REGEX REPLACE "[.]png" ".inc.c" TEXTURE_INC_CS "${TEXTURE_PNGS}")
string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" TEXTURE_INC_CS "${TEXTURE_INC_CS}")

# Ending cake textures are generated in a special way
make_directory_target("${BUILD_TMP_DIR}/levels/ending")
add_custom_command(
    OUTPUT "${BUILD_TMP_DIR}/levels/ending/cake.inc.c"
    COMMAND "${SKYCONV_COMMAND}" "${SKYCONV_ARGS}"
        "--type" "cake" "--split"
        "${CMAKE_CURRENT_SOURCE_DIR}/levels/ending/cake.png"
        "${BUILD_TMP_DIR}/levels/ending"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/levels/ending/cake.png"
        "${SKYCONV_TARGET}"
        "${BUILD_TMP_DIR}/levels/ending"
    COMMENT "Preprocessing cake.png"
    VERBATIM
)
list(APPEND TEXTURE_INC_CS "${BUILD_TMP_DIR}/levels/ending/cake.inc.c")

set(TEXTURE_PNG_DIRS)

foreach(TEXTURE_PNG ${TEXTURE_PNGS})
    # Get the useful parts of the texture name/path.

    get_filename_component(TEXTURE_DIRECTORY "${TEXTURE_PNG}" DIRECTORY)
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_DIRECTORY "${TEXTURE_DIRECTORY}")
    
    string(REGEX REPLACE "[.]png$" "" TEXTURE_BASE_PATH "${TEXTURE_PNG}")
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_BASE_PATH "${TEXTURE_BASE_PATH}")
    
    # RGBA32, RGBA16, IA16, IA8, IA4, IA1, I8, I4
    string(REGEX REPLACE "^.*\\.([a-z0-9]*).png$" "\\1" TEXTURE_FMT "${TEXTURE_PNG}")
    string(REGEX REPLACE "/" "_" TEXTURE_TARGET_BASE "${TEXTURE_BASE_PATH}")

    make_directory_target("${BUILD_TMP_DIR}${TEXTURE_DIRECTORY}")

    list(APPEND TEXTURE_PNG_DIRS "${TEXTURE_DIRECTORY}")

    if(EXTERNAL_DATA)

        # Convert the filename into a hex dump in C.
        add_custom_command(
            OUTPUT "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            COMMAND "${MKHEADER_COMMAND}" "-e" "-x"
                "${TEXTURE_BASE_PATH}"
                "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            DEPENDS
                "${MKHEADER_TARGET}"
                "${BUILD_TMP_DIR}${TEXTURE_DIRECTORY}"
            COMMENT "Translating ${TEXTURE_BASE_PATH}"
            VERBATIM
        )

    else()
    
        # Convert the PNG into correct format using the correct tool.
        set(TEXTURE_N64GRAPHICS_COMMAND "${N64GRAPHICS_COMMAND}")
        set(TEXTURE_N64GRAPHICS_TARGET "${N64GRAPHICS_TARGET}")
        if(TEXTURE_FMT MATCHES "ci8" OR TEXTURE_FMT MATCHES "ci4")
            set(TEXTURE_N64GRAPHICS_COMMAND "${N64GRAPHICS_CI_COMMAND}")
            set(TEXTURE_N64GRAPHICS_TARGET "${N64GRAPHICS_CI_TARGET}")
        endif()
    
        add_custom_command(
            OUTPUT "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}"
            COMMAND
                "${TEXTURE_N64GRAPHICS_COMMAND}"
                    "-i" "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}"
                    "-g" "${TEXTURE_PNG}"
                    "-f" "${TEXTURE_FMT}"
            DEPENDS
                "${TEXTURE_PNG}"
                "${TEXTURE_N64GRAPHICS_TARGET}"
                "${BUILD_TMP_DIR}${TEXTURE_DIRECTORY}"
            COMMENT "Preprocessing ${TEXTURE_BASE_PATH}.png"
            VERBATIM
        )

        # Convert the converted PNG into a hex dump in C.
        add_custom_command(
            OUTPUT "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            COMMAND "${MKHEADER_COMMAND}" "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}" "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            DEPENDS "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}" "${MKHEADER_TARGET}"
            COMMENT "Translating ${TEXTURE_BASE_PATH}"
            VERBATIM
        )
    endif()
        
endforeach()

list(REMOVE_DUPLICATES TEXTURE_PNG_DIRS)

# These are includes, and will be pulled into other translation units.
add_custom_target("texture_inc_cs" ALL DEPENDS ${TEXTURE_INC_CS})

# -= Levels Header =-

make_directory_target("${BUILD_TMP_DIR}/include")

add_custom_command(
    OUTPUT "${BUILD_TMP_DIR}/include/level_headers.h"
    COMMAND 
        "${CPP_COMMAND}" ${CPP_ARGS}
            -I "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in" |
        "${PYTHON_EXECUTABLE}" "${OUTPUT_LEVEL_HEADERS_PY}" >
        "${BUILD_TMP_DIR}/include/level_headers.h"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in"
        "${OUTPUT_LEVEL_HEADERS_PY}"
        "${BUILD_TMP_DIR}/include"
    COMMENT "Preprocessing level_headers.h"
    VERBATIM)

add_custom_target("level_headers.h" ALL DEPENDS "${BUILD_TMP_DIR}/include/level_headers.h")

# -= Text/Course Data =-

set(TEXT_DATA_INC_C_FILES)
foreach(DATA_VERSION us)
    
    make_directory_target("${BUILD_TMP_DIR}/text/${DATA_VERSION}")

    add_custom_command(
        OUTPUT "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
        COMMAND
            "${CPP_COMMAND}" ${CPP_ARGS} ${VERSION_CFLAGS}
                "${CMAKE_CURRENT_SOURCE_DIR}/text/define_courses.inc.c"
                -o "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
                -I "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/" &&
            "${TEXTCONV_COMMAND}"
                "${CHARMAP_TXT_PATH}"
                "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
                "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/courses.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/text/define_courses.inc.c"
            "${TEXTCONV_TARGET}"
            "${CHARMAP_TXT_PATH}"
            "${BUILD_TMP_DIR}/text/${DATA_VERSION}"
        COMMENT "Preprocessing define_courses.inc.c"
        VERBATIM)
    list(APPEND TEXT_DATA_INC_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c")

    add_custom_command(
        OUTPUT "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
	    COMMAND
            "${CPP_COMMAND}" -P ${VERSION_CFLAGS}
                "${CMAKE_CURRENT_SOURCE_DIR}/text/define_text.inc.c" 
                -o "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
                -I "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/" &&
            "${TEXTCONV_COMMAND}"
                "${CHARMAP_TXT_PATH}"
                "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
                "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/text/define_text.inc.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/courses.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/dialogs.h"
            "${CHARMAP_TXT_PATH}"
            "${TEXTCONV_TARGET}"
            "${BUILD_TMP_DIR}/text/${DATA_VERSION}"
        COMMENT "Preprocessing define_text.inc.c"
        VERBATIM)
    list(APPEND TEXT_DATA_INC_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c")
endforeach()

set(LOCALIZE_HEADERS 
    "text_strings.h"
    "text_menu_strings.h"
    "text_options_strings.h")
set(LOCALIZE_HEADERS_OUT)

foreach(HEADER ${LOCALIZE_HEADERS})
    add_custom_command(
        OUTPUT "${BUILD_TMP_DIR}/include/${HEADER}"
        COMMAND
            "${TEXTCONV_COMMAND}"
                "${CHARMAP_TXT_PATH}"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/${HEADER}.in"
                "${BUILD_TMP_DIR}/include/${HEADER}"
        DEPENDS
            "${TEXTCONV_TARGET}"
            "${CHARMAP_TXT_PATH}"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/${HEADER}.in"
        COMMENT "Preprocessing ${HEADER}"
        VERBATIM)
    list(APPEND LOCALIZE_HEADERS_OUT "${BUILD_TMP_DIR}/include/${HEADER}")
endforeach()

add_custom_target("localize_text" ALL DEPENDS ${TEXT_DATA_INC_C_FILES} ${LOCALIZE_HEADERS_OUT})

# -= Executable Target =-

foreach(FLAG ${VERSION_CFLAGS} ${BACKEND_CFLAGS} ${GRUCODE_CFLAGS} ${MISC_CFLAGS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

add_executable("${TARGET}" "${DATA_C_FILES};${DATA_O_FILES};${C_FILES};${CXX_FILES};${LEVEL_C_FILES};${ULTRA_C_FILES};${GODDARD_C_FILES}")

set_source_files_properties(
    ${DATA_O_FILES}
    PROPERTIES
    EXTERNAL_OBJECT True
    GENERATED True
)

# -= Other Dependencies =-

if(MINGW)
    include(FindPkgConfig)
endif()

if(USE_PYTHON)
    set(Python_ADDITIONAL_VERSIONS 3.6)
    find_package(PythonLibs 3.6 REQUIRED)
    target_link_libraries("${TARGET}" ${PYTHON_LIBRARIES})
    target_include_directories("${TARGET}" PUBLIC ${PYTHON_INCLUDE_DIRS})
endif()

if(WINDOW_API MATCHES "SDL2")
    if(MINGW)
        pkg_check_modules(GLEW REQUIRED glew)
        target_include_directories("${TARGET}" PUBLIC ${GLEW_INCLUDE_DIRS})
        target_link_libraries("${TARGET}" ${GLEW_LIBRARIES} "opengl32")
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries("${TARGET}" OpenGL::GL)
    endif()

    if(MINGW)
        pkg_check_modules(SDL2 REQUIRED sdl2)
    else()
        find_package(SDL2 REQUIRED)
    endif()
    target_include_directories("${TARGET}" PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries("${TARGET}" ${SDL2_LIBRARIES})
elseif(RENDERER MATCHES "D3D11")
    target_link_libraries("${TARGET}" d3d11.lib)
elseif(RENDERER MATCHES "D3D12")
    target_link_libraries("${TARGET}" d3d12.lib)
endif()

target_include_directories("${TARGET}" PUBLIC ${INCLUDE_DIRS})

add_dependencies("${TARGET}" "texture_inc_cs")
add_dependencies("${TARGET}" "localize_text")
add_dependencies("${TARGET}" "level_headers.h")

install(
    TARGETS "${TARGET}"
    RUNTIME DESTINATION "${INSTALL_BIN_ROOT}"
)

if(USE_PYTHON)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/sm64.py" DESTINATION "${INSTALL_SCRIPT_ROOT}")
endif()

if(EXTERNAL_DATA)
    install(
        DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/actors"
            "${CMAKE_CURRENT_SOURCE_DIR}/levels"
            "${CMAKE_CURRENT_SOURCE_DIR}/textures"
        DESTINATION "${INSTALL_RES_ROOT}${EXT_TEXTURE_DIR}"
        FILES_MATCHING PATTERN "*.png"
    )
    install(
        DIRECTORY "${SKYTILE_DIR}"
        DESTINATION "${INSTALL_RES_ROOT}${EXT_TEXTURE_DIR}/textures"
        FILES_MATCHING PATTERN "*.png"
    )
    install(
        FILES
            "${BUILD_TMP_DIR}/sound/sequences.bin"
            "${BUILD_TMP_DIR}/sound/sound_data.ctl"
            "${BUILD_TMP_DIR}/sound/sound_data.tbl"
            "${BUILD_TMP_DIR}/sound/bank_sets"
        DESTINATION "${INSTALL_RES_ROOT}${EXT_SOUND_DIR}"
    )
endif()
