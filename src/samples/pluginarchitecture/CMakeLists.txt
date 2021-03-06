cmake_minimum_required(VERSION 2.6)

project(atoll-samples-plugin-architecture)

# Set the default build type to release
# NOTE: the "NMake Makefiles" generator will always set the default build type to "Debug"
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug, Release, RelWithDebInfo and MinSizeRel" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Define vars
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Init directories to search for CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../../cmake")
message(STATUS "")

# Set platform specific global options
if(WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

# Add the project groups
source_group("myapplication" ${CMAKE_SOURCE_DIR}/myapplication/*)
source_group("myengine" ${CMAKE_SOURCE_DIR}/myengine/*)
source_group("openglplugin" ${CMAKE_SOURCE_DIR}/openglplugin/*)
source_group("zipplugin" ${CMAKE_SOURCE_DIR}/zipplugin/*)

# myapplication files
set(myapplication_files
	${CMAKE_SOURCE_DIR}/myapplication/myapplication.cpp
	)

# myengine files
set(myengine_files
	${CMAKE_SOURCE_DIR}/myengine/Engine.cpp
	${CMAKE_SOURCE_DIR}/myengine/Plugin.cpp
	${CMAKE_SOURCE_DIR}/myengine/PluginKernel.cpp
	)

# openglplugin files
set(openglplugin_files
	${CMAKE_SOURCE_DIR}/openglplugin/OpenglPlugin.cpp
	)

# zipplugin files
set(zipplugin_files
	${CMAKE_SOURCE_DIR}/zipplugin/ZipPlugin.cpp
	)

# Set compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Set Gnu CXX compiler flags")
	set(CMAKE_CXX_FLAGS "-Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2") # Optimize
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEF_EnableAsserts")
	set(CMAKE_EXE_LINKER_FLAGS "-s") # Strip binary
endif()
if(MSVC)
	# Preprocessor definitions: semicolon-separated list
	set (export_definitions "_USRDLL;DEF_LibExport")
	# Adds -D define flags to the compilation of source files
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build application
add_executable(myapplication ${myapplication_files})
target_link_libraries(myapplication myengine)

# Build engine library
add_library(myengine SHARED ${myengine_files})
target_link_libraries(myengine -ldl)

# Build plugins
add_library(openglplugin SHARED ${openglplugin_files})
add_library(zipplugin SHARED ${zipplugin_files})

# Tests
enable_testing()
add_test(NAME test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/myapplication
	COMMAND ${CMAKE_BINARY_DIR}/bin/myapplication
	)
