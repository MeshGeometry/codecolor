cmake_minimum_required (VERSION 2.8.6)

project(codecolor C CXX)

if (UNIX)
  set (CMAKE_CXX_STANDARD 11)
endif ()

# build miniclib
add_subdirectory("./miniclib")

#add the current source
file(GLOB COLOR_SRC
	"./Source/*.h"
	"./Source/*.cpp"
	)

#include miniclib
include_directories("./miniclib/*")

#build this lib
add_library(codecolor ${COLOR_SRC})

#organize source
source_group("codecolor" FILES ${COLOR_SRC})

#add the test material
target_sources(minitests PUBLIC "../../Source/Test_CodeColor.cpp")



