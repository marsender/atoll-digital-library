# Copyright 2006-2008 The FLWOR Foundation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# - Try to find the libcppunit libraries
#
# Once done this will define
#
#  CPPUNIT_FOUND   - system has libcppunit
#  CPPUNIT_INCLUDE - the libcppunit include directory
#  CPPUNIT_LIBRARY - libcppunit library
#

# Define CppUnit min version
if(NOT CPPUNIT_MIN_VERSION)
  SET(CPPUNIT_MIN_VERSION 1.12.0)
endif(NOT CPPUNIT_MIN_VERSION)

FIND_PROGRAM(CPPUNIT_CONFIG_EXECUTABLE cppunit-config)

IF(CPPUNIT_CONFIG_EXECUTABLE)
	EXEC_PROGRAM(${CPPUNIT_CONFIG_EXECUTABLE} ARGS --cflags RETURN_VALUE _return_VALUE OUTPUT_VARIABLE CPPUNIT_CFLAGS)
	EXEC_PROGRAM(${CPPUNIT_CONFIG_EXECUTABLE} ARGS --libs RETURN_VALUE _return_VALUE OUTPUT_VARIABLE CPPUNIT_LIBRARY)
	STRING(REGEX REPLACE "-I(.+)" "\\1" CPPUNIT_CFLAGS "${CPPUNIT_CFLAGS}")
ENDIF(CPPUNIT_CONFIG_EXECUTABLE)

IF(NOT CPPUNIT_INCLUDE)
	IF (NOT CPPUNIT_INCLUDE_DIR)
		set(CPPUNIT_INCLUDE_DIR)
	ENDIF (NOT CPPUNIT_INCLUDE_DIR)
	find_path(
		CPPUNIT_INCLUDE
		NAMES cppunit/TestRunner.h
		PATHS ${CPPUNIT_INCLUDE_DIR})
ENDIF(NOT CPPUNIT_INCLUDE)

IF(NOT CPPUNIT_LIBRARY)
	IF (NOT CPPUNIT_LIBRARY_DIR)
		set(CPPUNIT_LIBRARY_DIR)
	ENDIF (NOT CPPUNIT_LIBRARY_DIR)
	find_library(
		CPPUNIT_LIBRARY
		NAMES cppunit
		PATHS ${CPPUNIT_LIBRARY_DIR})
ENDIF(NOT CPPUNIT_LIBRARY)

IF (CPPUNIT_INCLUDE AND CPPUNIT_LIBRARY)
	set(CPPUNIT_FOUND TRUE)
ELSE (CPPUNIT_INCLUDE AND CPPUNIT_LIBRARY)
	set(CPPUNIT_FOUND FALSE)
ENDIF (CPPUNIT_INCLUDE AND CPPUNIT_LIBRARY)

IF(CPPUNIT_FOUND)

	MESSAGE(STATUS "Found CppUnit library -- " ${CPPUNIT_LIBRARY})

  IF(CPPUNIT_CONFIG_EXECUTABLE)
    EXEC_PROGRAM(${CPPUNIT_CONFIG_EXECUTABLE} ARGS --version RETURN_VALUE _return_VALUE OUTPUT_VARIABLE CPPUNIT_INSTALLED_VERSION)
  ELSE(CPPUNIT_CONFIG_EXECUTABLE)
		# How can we find cppunit version?
		MESSAGE (STATUS "Ensure CppUnit installed version is at least ${CPPUNIT_MIN_VERSION}")
		SET (CPPUNIT_INSTALLED_VERSION ${CPPUNIT_MIN_VERSION})
  ENDIF(CPPUNIT_CONFIG_EXECUTABLE)

  COMPARE_VERSION_STRINGS("${CPPUNIT_INSTALLED_VERSION}" "${CPPUNIT_MIN_VERSION}" result)
  IF(result LESS 0)
    SET(CPPUNIT_FOUND FALSE)
    MESSAGE(WARNING "You need at least CppUnit version ${CPPUNIT_MIN_VERSION}")
  ENDIF(result LESS 0)

ELSE(CPPUNIT_FOUND)
	MESSAGE(STATUS "Could not find CppUnit library")
ENDIF(CPPUNIT_FOUND)
