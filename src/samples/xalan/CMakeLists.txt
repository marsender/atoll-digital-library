cmake_minimum_required(VERSION 2.6)

project(atoll-samples-xalan)

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
include(CompareVersionStrings)
message(STATUS "")

# Set platform specific global options
if(WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

# Xerces-C
message(STATUS "Looking for Xerces-C")
find_package(XercesC)
if(XERCESC_FOUND)
	COMPARE_VERSION_STRINGS("${XERCESC_VERSION}" 2.8.0 result)
	if(result LESS 0)
		message(FATAL_ERROR "Xerces-C ${XERCESC_VERSION} not supported. Make sure you have Xerces-C 2.8.0 or newer installed")
	else()
		include_directories(${XERCESC_INCLUDE})
	endif()
else()
	message(FATAL_ERROR "Xerces-C library is required")
endif()
message(STATUS "")

# Xalan
message(STATUS "Looking for Xalan")
find_package(Xalan)
if(XALAN_FOUND)
	include_directories(${XALAN_INCLUDE})
else()
	message(FATAL_ERROR "Xalan library is required")
endif()
message(STATUS "")

# Add the project groups
source_group("XalanTransform" ${CMAKE_SOURCE_DIR}/XalanTransform/*)

# XalanTransform files
set(xalantransform_files
	${CMAKE_SOURCE_DIR}/XalanTransform/XalanTransform.cpp
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
	# Adds -D define flags to the compilation of source files
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build
add_executable(xalantransform ${xalantransform_files})
target_link_libraries(xalantransform ${XERCESC_LIBRARY} ${XALAN_LIBRARY})

# Tests
enable_testing()
add_test(NAME test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/XalanTransform
	COMMAND ${CMAKE_BINARY_DIR}/bin/xalantransform foo.xml foo.xsl foo.html
	)
