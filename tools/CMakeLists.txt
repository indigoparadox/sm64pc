
cmake_minimum_required(VERSION 3.0.0)
project(sm64pc_tools)

if(NOT DEFINED BUILD_TMP_DIR)
    set(BUILD_TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
endif()

#set(tools_CFLAGS "-Llib -Iinclude -I../include -I . -Wall -Wextra -Wno-unused-parameter $(OSX_BUILD) -pedantic -std=c99 $(OPT_FLAG) -s")
set(tools_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# -= n64graphics =-

set(n64graphics_SOURCES n64graphics.c utils.c)
set(n64graphics_DEFINES -DN64GRAPHICS_STANDALONE)

add_executable("n64graphics" ${n64graphics_SOURCES})
target_compile_definitions("n64graphics" PUBLIC ${n64graphics_DEFINES})
#target_link_directories("n64graphics" PUBLIC ${tools_LIB_DIRS})
target_include_directories("n64graphics" PUBLIC ${tools_INCLUDE_DIRS})

# -= textconv =-

set(textconv_SOURCES textconv.c utf8.c hashtable.c)

add_executable("textconv" ${textconv_SOURCES})
#add_executable("${BUILD_TMP_DIR}/textconv" ALIAS "textconv")

#set(iplfontutil_SOURCES iplfontutil.c)

set(aifc_decode_SOURCES aifc_decode.c)

# -= aiff_extract_codebook =-

set(aiff_extract_codebook_SOURCES aiff_extract_codebook.c)

add_executable("aiff_extract_codebook" ${aiff_extract_codebook_SOURCES})
#target_compile_definitions("n64graphics" PUBLIC ${n64graphics_CFLAGS})
#target_link_libraries("aiff_extract_codebook" PUBLIC m)
#target_include_directories("aiff_extract_codebook" PUBLIC ${tools_INCLUDE_DIRS})

#set(tabledesign_SOURCES sdk-tools/tabledesign/codebook.c sdk-tools/tabledesign/estimate.c sdk-tools/tabledesign/print.c sdk-tools/tabledesign/tabledesign.c
#tabledesign_CFLAGS := -Wno-uninitialized -Iaudiofile -Laudiofile -laudiofile -lstdc++ -lm

# -= vadpcm_enc =-

set(vadpcm_enc_SOURCES sdk-tools/adpcm/vadpcm_enc.c sdk-tools/adpcm/vpredictor.c sdk-tools/adpcm/quant.c sdk-tools/adpcm/util.c sdk-tools/adpcm/vencode.c)

add_executable("vadpcm_enc" ${vadpcm_enc_SOURCES})

#extract_data_for_mio_SOURCES := extract_data_for_mio.c
#extract_data_for_mio_CFLAGS := $(OPT_FLAG)

# -= skyconv =-

set(skyconv_SOURCES skyconv.c n64graphics.c utils.c)
set(skyconv_CFLAGS -lm)

add_executable("skyconv" ${skyconv_SOURCES})
#target_compile_definitions("n64graphics" PUBLIC ${n64graphics_CFLAGS})
target_link_libraries("skyconv" PUBLIC m)
target_include_directories("skyconv" PUBLIC ${tools_INCLUDE_DIRS})
