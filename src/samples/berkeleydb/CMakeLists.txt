cmake_minimum_required(VERSION 2.6)

project(atoll-samples-berkeleydb)

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

# BDB
message(STATUS "Looking for BDB")
find_package(BDB)
if(BDB_FOUND)
	include_directories(${BDB_INCLUDE})
else()
	MESSAGE(FATAL_ERROR "BDB library is required")
endif()
message(STATUS "")

# Add the project groups
source_group("DatabaseLoad" ${CMAKE_SOURCE_DIR}/DatabaseLoad/*)

# DatabaseLoad files
set(databaseload_files
	${CMAKE_SOURCE_DIR}/DatabaseLoad/excxx_example_database_load.cpp
	${CMAKE_SOURCE_DIR}/DatabaseLoad/MyDb.cpp
	)
if(WIN32)
	set(databaseload_files ${databaseload_files} ${CMAKE_SOURCE_DIR}/DatabaseLoad/getopt.cpp)
endif(WIN32)

# Set compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Set Gnu CXX compiler flags")
	set(CMAKE_CXX_FLAGS "-Wall")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2") # Optimize
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEF_EnableAsserts")
	set(CMAKE_EXE_LINKER_FLAGS "-s") # Strip binary
endif()
if(MSVC)
	# Adds -D define flags to the compilation of source files
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build
add_executable(databaseload ${databaseload_files})
target_link_libraries(databaseload ${BDB_LIBRARY})

# Tests
enable_testing()
add_test(NAME test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/DatabaseLoad
	COMMAND ${CMAKE_BINARY_DIR}/bin/databaseload
	)
