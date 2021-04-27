
cmake_minimum_required(VERSION 3.0.0)
project(sm64pc_tools)

set(CMAKE_C_STANDARD 99)

add_subdirectory("audiofile")

include(CheckLibraryExists)

CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIB_M)

if(NOT DEFINED BUILD_TMP_DIR)
    set(BUILD_TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
endif()

set(tools_INCLUDE_DIRS 
   "${CMAKE_CURRENT_SOURCE_DIR}/include"
   "${CMAKE_CURRENT_SOURCE_DIR}/../include")

set(tools_DEFINES
   "_CRT_SECURE_NO_WARNINGS")

# -= mkheader =-

add_executable("mkheader" "mkheader.c")

# -= n64graphics =-

set(n64graphics_SOURCES n64graphics.c utils.c)
set(n64graphics_DEFINES -DN64GRAPHICS_STANDALONE)

add_executable("n64graphics" ${n64graphics_SOURCES})
target_compile_definitions("n64graphics" PUBLIC ${n64graphics_DEFINES})
target_include_directories("n64graphics" PUBLIC ${tools_INCLUDE_DIRS})
target_compile_definitions("n64graphics" PUBLIC ${tools_DEFINES})

# -= n64graphics_ci =-

set(n64graphics_ci_SOURCES n64graphics_ci_dir/n64graphics_ci.c n64graphics_ci_dir/exoquant/exoquant.c n64graphics_ci_dir/utils.c)

add_executable("n64graphics_ci" ${n64graphics_ci_SOURCES})
target_include_directories("n64graphics_ci" PUBLIC ${tools_INCLUDE_DIRS})
if(HAVE_LIB_M)
   target_link_libraries("n64graphics_ci" PUBLIC m)
endif() 

# -= textconv =-

set(textconv_SOURCES textconv.c utf8.c hashtable.c)

add_executable("textconv" ${textconv_SOURCES})
target_compile_definitions("textconv" PUBLIC ${tools_DEFINES})

# -= tabledesign =-

set(tabledesign_SOURCES
   "sdk-tools/tabledesign/codebook.c"
   "sdk-tools/tabledesign/estimate.c"
   "sdk-tools/tabledesign/print.c"
   "sdk-tools/tabledesign/tabledesign.c")
set(tabledesign_CFLAGS -Wno-uninitialized -Iaudiofile -Laudiofile -laudiofile -lstdc++ -lm)

add_executable("tabledesign" ${tabledesign_SOURCES})
target_link_libraries("tabledesign" "audiofile")

# -= aiff_extract_codebook =-

set(aiff_extract_codebook_SOURCES aiff_extract_codebook.c)

add_executable("aiff_extract_codebook" ${aiff_extract_codebook_SOURCES})
target_compile_definitions("aiff_extract_codebook" PUBLIC ${tools_DEFINES})
add_dependencies("aiff_extract_codebook" "tabledesign")

# -= vadpcm_enc =-

set(vadpcm_enc_SOURCES sdk-tools/adpcm/vadpcm_enc.c sdk-tools/adpcm/vpredictor.c sdk-tools/adpcm/quant.c sdk-tools/adpcm/util.c sdk-tools/adpcm/vencode.c)

add_executable("vadpcm_enc" ${vadpcm_enc_SOURCES})
target_compile_definitions("vadpcm_enc" PUBLIC ${tools_DEFINES})

# -= skyconv =-

set(skyconv_SOURCES skyconv.c n64graphics.c utils.c)
set(skyconv_CFLAGS -lm)

add_executable("skyconv" ${skyconv_SOURCES})
if(HAVE_LIB_M)
   target_link_libraries("skyconv" PUBLIC m)
endif() 
target_include_directories("skyconv" PUBLIC ${tools_INCLUDE_DIRS})
target_compile_definitions("skyconv" PUBLIC ${tools_DEFINES})

