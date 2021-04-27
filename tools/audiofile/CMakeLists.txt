
cmake_minimum_required(VERSION 3.0.0)
project(audiofile)

set(CMAKE_CXX_STANDARD 11)

add_library("audiofile" audiofile.cpp audiofile.h aupvlist.h)
target_include_directories("audiofile" PUBLIC ".")

#libaudiofile.a: audiofile.o
#	ar rcs libaudiofile.a audiofile.o

#audiofile.o: audiofile.cpp audiofile.h aupvlist.h
#	$(CXX) -std=c++11 -DNDEBUG -fno-rtti -fno-exceptions -fvisibility-inlines-hidden -O2 -I. -c audiofile.cpp
