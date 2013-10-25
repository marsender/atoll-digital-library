cmake_minimum_required(VERSION 2.6)

project(atoll-samples-icu)

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

# ICU
message(STATUS "Looking for ICU")
find_package(ICU)
if(ICU_FOUND AND ICU_IO_FOUND AND ICU_I18N_FOUND AND ICU_DATA_FOUND)
	include_directories(${ICU_INCLUDE})
else()
	MESSAGE(FATAL_ERROR "ICU library is required")
endif()
message(STATUS "")

# Add the project groups
source_group("Collation" ${CMAKE_SOURCE_DIR}/Collation/*)

# Collation files
set(collation_files
	${CMAKE_SOURCE_DIR}/Collation/Collation.cpp
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
add_executable(collation ${collation_files})
target_link_libraries(collation ${ICU_LIBRARY} ${ICU_IO_LIBRARIES} ${ICU_I18N_LIBRARIES} ${ICU_DATA_LIBRARIES})

# Tests
enable_testing()
add_test(NAME test
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Collation
	COMMAND ${CMAKE_BINARY_DIR}/bin/collation
	)
