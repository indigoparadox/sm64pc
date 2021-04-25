cmake_minimum_required(VERSION 3.0.0)
project(sm64pc VERSION 0.1.0)
enable_language(C ASM)

# Be quiet!
#function(message)
#  list(GET ARGV 0 MessageType)
#  if(MessageType STREQUAL FATAL_ERROR OR
#     MessageType STREQUAL SEND_ERROR OR
#     MessageType STREQUAL WARNING OR
#     MessageType STREQUAL AUTHOR_WARNING)
#    list(REMOVE_AT ARGV 0)
#    _message(${MessageType} "${ARGV}")
#  endif()
#endfunction()

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
set(NON_MATCHING True)
# Build and optimize for Raspberry Pi(s)
#set(TARGET_RPI ?= 0
# Build with python support.
set(USE_PYTHON True)
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

if(NOT DEFINED BUILD_TEXTURES)
    set(BUILD_TEXTURES True)
endif()

set(BUILD_TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
message(STATUS "Using ${BUILD_TMP_DIR} for intermediate build files.")

set(BUILD_TMP_DIR_CHILDREN
    "${BUILD_TMP_DIR}/sound"
    "${BUILD_TMP_DIR}/sound/samples"
    "${BUILD_TMP_DIR}/sound/sequences"
    "${BUILD_TMP_DIR}/include"
    "${BUILD_TMP_DIR}/text"
    "${BUILD_TMP_DIR}/levels"
    "${BUILD_TMP_DIR}/levels/ending"
    "${BUILD_TMP_DIR}/text/us"
    "${BUILD_TMP_DIR}/bin")

set(TARGET)
Set(INCLUDE_DIRS
    "${BUILD_TMP_DIR}"
    "${BUILD_TMP_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(VERSION_CFLAGS)

if("${VERSION}" MATCHES "us")
    list(APPEND VERSION_CFLAGS "-DVERSION_US")
    #set(VERSION_AS_FLAGS "--defsym VERSION_US=1")
    set(GRUCODE_CFLAGS "-DF3D_OLD")
    set(GRUCODE_ASFLAGS "--defsym F3D_OLD=1")
    set(TARGET "sm64.us")
endif()

if(${USE_PYTHON})
    list(APPEND VERSION_CFLAGS "-DUSE_PYTHON")
endif()

if(WIN32 OR UNIX)
    list(APPEND VERSION_CFLAGS "-DAVOID_UB")
    set(NON_MATCHING True)
endif()

if(${NON_MATCHING})
    list(APPEND VERSION_CFLAGS "-DNON_MATCHING")
endif()

if(NOT TARGET_BITS EQUAL 0)
  set(BITS "-m${TARGET_BITS}")
endif()

# TODO: Figure out how to make this smarter.
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

if("${GRUCODE}" MATCHES "f3dex2e")
    set(GRUCODE_CFLAGS "-DF3DEX_GBI")
    set(GRUCODE_ASFLAGS "--defsym F3DEX_GBI_SHARED=1 --defsym F3DEX_GBI=1")
    set(TARGET "${TARGET}.f3dex")
    #COMPARE := 0
endif()

set(SKYTILE_DIR "${BUILD_TMP_DIR}/textures/skybox_tiles")

set(SOUND_DATA_CTL_PATH "${BUILD_TMP_DIR}/sound/sound_data.ctl")
set(SOUND_DATA_TBL_PATH "${BUILD_TMP_DIR}/sound/sound_data.tbl")
set(SEQUENCES_BIN_PATH "${BUILD_TMP_DIR}/sound/sequences.bin")
set(BANK_SETS_PATH "${BUILD_TMP_DIR}/sound/bank_sets")

#set(ENDIAN_BITWIDTH "${BUILD_TMP_DIR}/endian-and-bitwidth")

set(TEXTCONV_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/textconv")
set(N64GRAPHICS_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/n64graphics")
set(SKYCONV_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/skyconv")
set(SKYCONV_ARGS "--store-names --write-tiles \"${SKYTILE_DIR}\"")
set(AIFF_EXTRACT_CODEBOOK "${CMAKE_CURRENT_SOURCE_DIR}/tools/aiff_extract_codebook")
set(VADPCM_ENC_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/vadpcm_enc")
set(HEXDUMP_COMMAND "hexdump")
set(HEXDUMP_ARGS "-v" "-e" "1/1 \"0x%X,\"")
set(ZEROTERM_COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tools/zeroterm.py")

set(DATA_C_FILES)
set(DATA_O_FILES)

macro(hexdump_binary_assets bin_file c_file array_name array_sz)
    add_custom_command(
        OUTPUT "${c_file}"
        COMMAND 
            "echo" "unsigned char ${array_name}[${array_sz}] = {" > "${c_file}" &&
            "${HEXDUMP_COMMAND}" ${HEXDUMP_ARGS} "${bin_file}" >> 
                "${c_file}" &&
            "echo" "};" >> "${c_file}"
        MAIN_DEPENDENCY "${bin_file}"
        COMMENT "Generating ${c_file} from ${bin_file}"
        VERBATIM
    )
    list(APPEND DATA_C_FILES "${c_file}")
    message(STATUS "DATA ${DATA_C_FILES}")
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
set(BACKEND_LDFLAGS)
set(SDL2_USED 0)

# For now, it's either SDL+GL or DXGI+DirectX, so choose based on WAPI.
#if(${WINDOW_API} EQUAL DXGI)
#    set(DXBITS `cat $(ENDIAN_BITWIDTH) | tr ' ' '\n' | tail -1`
#  ifeq ($(RENDER_API),D3D11)
#    BACKEND_LDFLAGS += -ld3d11
#  else ifeq ($(RENDER_API),D3D12)
#    BACKEND_LDFLAGS += -ld3d12
#    BACKEND_CFLAGS += -Iinclude/dxsdk
#  endif
#  BACKEND_LDFLAGS += -ld3dcompiler -ldxgi -ldxguid
#  BACKEND_LDFLAGS += -lsetupapi -ldinput8 -luser32 -lgdi32 -limm32 -lole32 -loleaut32 -lshell32 -lwinmm -lversion -luuid -static
#if("${WINDOW_API}" EQUAL SDL2)
    #if(${WIN32})
    #    set(BACKEND_LDFLAGS "${BACKEND_LDFLAGS} -lglew32 -lglu32 -lopengl32")
    #elseif(${TARGET_RPI} EQUAL 1)
    #    set(BACKEND_LDFLAGS "${BACKEND_LDFLAGS} -lGLESv2")
    #elseif(${OSX_BUILD),1)
    #BACKEND_LDFLAGS += -framework OpenGL `pkg-config --libs glew`
    #else
    #    BACKEND_LDFLAGS += -lGL
    #endif
 #   set(SDL_USED 2)
 #   find_package(OpenGL)
#endif()

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

# -= Data File Lists =-

file(GLOB SOUND_BANK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sound/sound_banks/*.json")

if(${VERSION} MATCHES "sh")
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
    list(APPEND BUILD_TMP_DIR_CHILDREN ${SAMPLE_BUILD_DIR})

    file(GLOB DIR_AIFFS "${SAMPLE_DIR}/*.aiff")

    foreach(AIFF ${DIR_AIFFS})
        string(REGEX REPLACE "[.]aiff$" ".aifc" AIFC "${AIFF}")
        string(REGEX REPLACE "[.]aiff$" ".table" TABLE "${AIFF}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" AIFC "${AIFC}")
        string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" TABLE "${TABLE}")

        list(APPEND SOUND_SAMPLE_AIFCS ${AIFC})
        
        add_custom_command(
            OUTPUT "${TABLE}"
            COMMAND "${AIFF_EXTRACT_CODEBOOK}" "${AIFF}" > "${TABLE}"
            DEPENDS "${AIFF_EXTRACT_CODEBOOK}" "${AIFF}"
            COMMENT "Encoding ${AIFF}"
            VERBATIM
        )
    
        add_custom_command(
            OUTPUT "${AIFC}"
            COMMAND "${VADPCM_ENC_COMMAND}" -c "${TABLE}" "${AIFF}" "${AIFC}"
            DEPENDS "${VADPCM_ENC_COMMAND}" "${AIFF}" "${TABLE}"
            COMMENT "Encoding ${AIFF}"
            VERBATIM
        )
    endforeach()
endforeach()

# -= Build Directory Creation =-

foreach(CHILD in ${BUILD_TMP_DIR_CHILDREN})
    message(STATUS "Creating subdirectory ${CHILD}...")
    file(MAKE_DIRECTORY "${CHILD}")
endforeach()

# -= Assets Extraction =-

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

#foreach(AIFC ${SOUND_SAMPLE_AIFCS})
#    string(REGEX REPLACE "[.]aifc$" ".aiff" AIFF "${AIFC}")
#    add_custom_command(
#        OUTPUT "${AIFC}"
#        COMMAND "${VADPCM_ENC}" -c "${AIFF}" "${AIFC}"
#        DEPENDS "${VADPCM_ENC}" "${AIFF}"
#        COMMENT "Encoding ${AIFF}"
#        VERBATIM
#    )
#endforeach()

#$(BUILD_DIR)/%.table: %.aiff
#	$(AIFF_EXTRACT_CODEBOOK) $< >$@

#$(BUILD_DIR)/%.aifc: $(BUILD_DIR)/%.table %.aiff
#	$(VADPCM_ENC) -c $^ $@

add_custom_command(
    OUTPUT "${SOUND_DATA_CTL_PATH}"
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
#add_custom_target("sound_data.ctl" ALL DEPENDS "${SOUND_DATA_CTL_PATH}")
#add_dependencies("${TARGET}" "sound_data.ctl")

foreach(S_FILE ${SOUND_SEQUENCE_S_FILES})
    string(REGEX REPLACE "[.]s$" ".m64" S_M64_FILE "${S_FILE}")
    string(REGEX REPLACE "[.]s$" ".o" S_O_FILE "${S_FILE}")
    string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${BUILD_TMP_DIR}" S_M64_FILE "${S_M64_FILE}")
    list(APPEND SOUND_SEQUENCE_FILES "${S_M64_FILE}")
    add_custom_command(
        OUTPUT "${S_O_FILE}"
        COMMAND "${CMAKE_ASM_COMPILER}" ${CMAKE_ASFLAGS} "-o" "${S_O_FILE}" "${S_FILE}"
        MAIN_DEPENDENCY "${S_FILE}"
        COMMENT "Assembling ${S_FILE}"
        VERBATIM
    )
    add_custom_command(
        OUTPUT "${S_M64_FILE}"
        COMMAND "${CMAKE_OBJCOPY}" "-j" ".rodata" "${S_O_FILE}" "-O" "binary" "${S_M64_FILE}"
        MAIN_DEPENDENCY "${S_O_FILE}"
        COMMENT "Translating ${S_O_FILE}"
        VERBATIM
    )
endforeach()
message(STATUS ${SOUND_SEQUENCE_FILES})

if(${EXTERNAL_DATA} EQUAL 1)

else()

    if(${VERSION} MATCHES "sh")
        # $(SOUND_BIN_DIR)/sequences.bin: $(SOUND_BANK_FILES) sound/sequences.json sound/sequences/ sound/sequences/jp/ $(SOUND_SEQUENCE_FILES) $(ENDIAN_BITWIDTH)
        # 	$(PYTHON) tools/assemble_sound.py --sequences $@ $(SOUND_BIN_DIR)/bank_sets sound/sound_banks/ sound/sequences.json $(SOUND_SEQUENCE_FILES) $(VERSION_CFLAGS) $$(cat $(ENDIAN_BITWIDTH))
    else()
        add_custom_command(
            OUTPUT "${SEQUENCES_BIN_PATH}"
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
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tools/assemble_sound.py" ${SOUND_SEQUENCE_FILES}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Preprocessing sound_data.ctl"
            VERBATIM
        )
    endif()

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

# $(BUILD_DIR)/levels/scripts.o: $(BUILD_DIR)/include/level_headers.h

# $(BUILD_DIR)/include/level_headers.h: levels/level_headers.h.in
# 	$(CPP) -I . levels/level_headers.h.in | $(PYTHON) tools/output_level_headers.py > $(BUILD_DIR)/include/level_headers.h

# $(BUILD_DIR)/assets/mario_anim_data.c: $(wildcard assets/anims/*.inc.c)
# 	$(PYTHON) tools/mario_anims_converter.py > $@

# $(BUILD_DIR)/assets/demo_data.c: assets/demo_data.json $(wildcard assets/demos/*.bin)
# 	$(PYTHON) tools/demo_data_converter.py assets/demo_data.json $(VERSION_CFLAGS) > $@

# -= Texture Generation =-

file(GLOB SKYBOX_PNGS "${CMAKE_CURRENT_SOURCE_DIR}/textures/skyboxes/*.png")

# Build skybox textures.
#foreach(SKYBOX ${SKYBOX_PNGS})
#    get_filename_component(SKYBOX_BASENAME "${SKYBOX}" NAME_WE)
#    set(SKYBOX_DEST "${BUILD_TMP_DIR}/bin/${SKYBOX_BASENAME}_skybox.o")
#    if(EXTERNAL_DATA EQUAL 1)
#        add_custom_command(
#            OUTPUT "${SKYBOX_BASENAME}_skybox.o"
#            COMMAND "${PYTHON_EXECUTABLE}" "${ZEROTERM_COMMAND}" "${SKYBOX}"
#            COMMENT "Preprocessing ${SKYBOX}"
#        )
#    else()
#        add_custom_command(
#            OUTPUT "${SKYBOX_BASENAME}_skybox.o"
#            COMMAND "${N64GRAPHICS_COMMAND}" "-i" "${SKYBOX_DEST}" "-g" "${SKYBOX}"
#            COMMENT "Preprocessing ${SKYBOX}"
#        )
#        list(APPEND DATA_O_FILES "${SKYBOX_DEST}")
#    endif()
#endforeach()

set(SKYBOX_C_FILES)
foreach(SKYBOX_PNG_PATH ${SKYBOX_PNGS})
    get_filename_component(SKYBOX_BASENAME "${SKYBOX_PNG_PATH}" NAME_WE)
    set(SKYBOX_C_PATH "${BUILD_TMP_DIR}/bin/${SKYBOX_BASENAME}_skybox.c")
    add_custom_command(
        OUTPUT "${SKYBOX_C_PATH}"
        COMMAND "${SKYCONV_COMMAND}" "${SKYCONV_ARGS}" "--type" "sky" "--split"
            "${SKYBOX_PNG_PATH}"
            "${BUILD_TMP_DIR}/bin"
        MAIN_DEPENDENCY "${SKYBOX_PNG_PATH}"
        COMMENT "Preprocessing ${SKYBOX_PNG_PATH}"
        VERBATIM
    )
    list(APPEND DATA_C_FILES "${SKYBOX_C_PATH}")
endforeach()

# Ending cake textures are generated in a special way
add_custom_command(
    OUTPUT "${BUILD_TMP_DIR}/levels/ending/cake.inc.c"
    COMMAND "${SKYCONV_COMMAND}" "${SKYCONV_ARGS}" "--type" "cake" "--split" "${CMAKE_CURRENT_SOURCE_DIR}/levels/ending/cake.png" "${BUILD_TMP_DIR}/levels/ending"
    MAIN_DEPENDENCY "${TEXTURE_PNG}"
    COMMENT "Preprocessing ${TEXTURE_BASE_PATH}.png"
    VERBATIM
)

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

foreach(TEXTURE_PNG ${TEXTURE_PNGS})
    # Get the useful parts of the texture name/path.
    string(REGEX REPLACE "[.]png$" "" TEXTURE_BASE_PATH_FULL "${TEXTURE_PNG}")
    get_filename_component(TEXTURE_DIRECTORY_FULL "${TEXTURE_PNG}" DIRECTORY)
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_DIRECTORY "${TEXTURE_DIRECTORY_FULL}")
    string(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}" "" TEXTURE_BASE_PATH "${TEXTURE_BASE_PATH_FULL}")
    # RGBA32, RGBA16, IA16, IA8, IA4, IA1, I8, I4
    string(REGEX REPLACE "^.*\\.([a-z0-9]*).png$" "\\1" TEXTURE_FMT "${TEXTURE_PNG}")
    file(MAKE_DIRECTORY "${BUILD_TMP_DIR}${TEXTURE_DIRECTORY}")
    string(REGEX REPLACE "/" "_" TEXTURE_TARGET_BASE "${TEXTURE_BASE_PATH}")
    
    
    if(EXTERNAL_DATA EQUAL 1)
        #$(BUILD_DIR)/%: %.png
        #	$(ZEROTERM) "$(patsubst %.png,%,$^)" > $@
    else()
    
        # Convert the PNG into correct format.
        add_custom_command(
            OUTPUT "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}"
            COMMAND "${N64GRAPHICS_COMMAND}" "-i" "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}" "-g" "${TEXTURE_PNG}" "-f" "${TEXTURE_FMT}"
            MAIN_DEPENDENCY "${TEXTURE_PNG}"
            COMMENT "Preprocessing ${TEXTURE_BASE_PATH}.png"
            VERBATIM
        )
        
        # Convert the converted PNG into a hex dump in C.
        add_custom_command(
            OUTPUT "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            COMMAND "${HEXDUMP_COMMAND}" ${HEXDUMP_ARGS} "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}" > "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}.inc.c"
            MAIN_DEPENDENCY "${BUILD_TMP_DIR}${TEXTURE_BASE_PATH}"
            COMMENT "Translating ${TEXTURE_BASE_PATH}"
            VERBATIM
        )
    endif()
endforeach()

# Add the converted texture to target deps.
add_custom_target("texture_inc_cs" ALL DEPENDS ${TEXTURE_INC_CS} "${BUILD_TMP_DIR}/levels/ending/cake.inc.c")

# -= Levels Header =-

add_custom_command(
    OUTPUT "${BUILD_TMP_DIR}/include/level_headers.h"
    COMMAND cpp -P -I "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in" | "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/output_level_headers.py" > "${BUILD_TMP_DIR}/include/level_headers.h"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/levels/level_headers.h.in"
    COMMENT "Preprocessing level_headers.h"
    VERBATIM)
add_custom_target("level_headers" ALL DEPENDS "${BUILD_TMP_DIR}/include/level_headers.h")

# -= Text/Course Data =-

set(TEXT_DATA_INC_C_FILES)
foreach(DATA_VERSION us)
    add_custom_command(
        OUTPUT "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
        COMMAND cpp -P ${VERSION_CFLAGS} "${CMAKE_CURRENT_SOURCE_DIR}/text/define_courses.inc.c" -o "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c" -I "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/" && "${TEXTCONV_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/charmap.txt" "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c" "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/courses.h"
        COMMENT "Preprocessing define_courses.inc.c"
        VERBATIM)
    #add_custom_target("define_courses.inc.c" ALL DEPENDS "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c")
    #add_dependencies("${TARGET}" "define_courses.inc.c")
    #list(APPEND DATA_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c")

    list(APPEND TEXT_DATA_INC_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_courses.inc.c")

    add_custom_command(
        OUTPUT "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
        DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/text/define_text.inc.c" "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/courses.h" "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/dialogs.h"
	    COMMAND cpp -P ${VERSION_CFLAGS} "${CMAKE_CURRENT_SOURCE_DIR}/text/define_text.inc.c" -o "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c" -I "${CMAKE_CURRENT_SOURCE_DIR}/text/${DATA_VERSION}/" && "${TEXTCONV_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/charmap.txt" "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c" "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c"
        COMMENT "Preprocessing define_text.inc.c"
        VERBATIM)
    #add_custom_target("define_text.inc.c" ALL DEPENDS "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c")
    #add_dependencies("${TARGET}" "define_text.inc.c")
    #list(APPEND DATA_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c")

    list(APPEND TEXT_DATA_INC_C_FILES "${BUILD_TMP_DIR}/text/${DATA_VERSION}/define_text.inc.c")
endforeach()

add_custom_target("text_data_inc_cs" ALL DEPENDS ${TEXT_DATA_INC_C_FILES})

# -= Localized Headers =-

set(LOCALIZE_HEADERS 
"text_strings.h"
"text_menu_strings.h"
"text_options_strings.h")

foreach(HEADER ${LOCALIZE_HEADERS})
    add_custom_target(
        ${HEADER}
        COMMAND "${TEXTCONV_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/charmap.txt" "${CMAKE_CURRENT_SOURCE_DIR}/include/${HEADER}.in" "${BUILD_TMP_DIR}/include/${HEADER}"
        BYPRODUCTS "${BUILD_TMP_DIR}/include/${HEADER}"
        RESULT_VARIABLE TEXTCONV_RESULT)
endforeach()

# -= Executable Target =-

foreach(FLAG ${VERSION_CFLAGS} ${BACKEND_CFLAGS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BACKEND_CFLAGS} ${VERSION_CFLAGS}")
#separate_arguments(VERSION_CFLAGS)

add_executable("${TARGET}" "${DATA_O_FILES};${C_FILES};${CXX_FILES};${LEVEL_C_FILES};${ULTRA_C_FILES};${DATA_C_FILES}")

set_source_files_properties(
    ${DATA_O_FILES}
    PROPERTIES
    EXTERNAL_OBJECT True
    GENERATED True
)

if(${USE_PYTHON})
    set(Python_ADDITIONAL_VERSIONS 3.6)
    find_package(PythonLibs 3.6 REQUIRED)
    target_link_libraries("${TARGET}" ${PYTHON_LIBRARIES})
    target_include_directories("${TARGET}" PUBLIC ${PYTHON_INCLUDE_DIRS})
endif()

if("${WINDOW_API}" MATCHES "SDL2")
    #set(SDL_USED 2)
    find_package(OpenGL REQUIRED)
    target_link_libraries("${TARGET}" OpenGL::GL)
    #target_include_directories("${TARGET}" OpenGL::GL)

    find_package(SDL2 REQUIRED)
    target_include_directories("${TARGET}" PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries("${TARGET}" ${SDL2_LIBRARIES})
endif()

target_include_directories("${TARGET}" PUBLIC ${INCLUDE_DIRS})

add_dependencies("${TARGET}" "texture_inc_cs")
add_dependencies("${TARGET}" "level_headers")
add_dependencies("${TARGET}" "text_data_inc_cs")

#GENERATED_C_FILES := $(BUILD_DIR)/assets/mario_anim_data.c $(BUILD_DIR)/assets/demo_data.c \
#  $(addprefix $(BUILD_DIR)/bin/,$(addsuffix _skybox.c,$(notdir $(basename $(wildcard textures/skyboxes/*.png)))))

#TODO: ifeq ($(EXTERNAL_DATA),1)

# Add localized text headers.
foreach(HEADER ${LOCALIZE_HEADERS})
    add_dependencies("${TARGET}" ${HEADER})
endforeach()
#target_include_directories("${TARGET}" PUBLIC "${BUILD_TMP_DIR}/include")
#foreach(INCLUDE_DIR in ${INCLUDE_DIRS})
#    target_include_directories("${TARGET}" PUBLIC "${INCLUDE_DIR}")
#endforeach()
