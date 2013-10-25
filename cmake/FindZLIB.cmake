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
# - Try to find the native ZLIB includes and library
#
# Once done this will define
#
#  ZLIB_FOUND   - True if zlib found.
#  ZLIB_INCLUDE - where to find zlib.h, etc.
#  ZLIB_LIBRARY - List of libraries when using zlib.
#

IF (ZLIB_INCLUDE)
  # Already in cache, be silent
  SET(ZLIB_FIND_QUIETLY TRUE)
ENDIF (ZLIB_INCLUDE)

# Look for the header file
IF (NOT ZLIB_INCLUDE_DIR)
	set(ZLIB_INCLUDE_DIR)
ENDIF (NOT ZLIB_INCLUDE_DIR)
find_path(
	ZLIB_INCLUDE
	NAMES zlib.h
	PATHS ${ZLIB_INCLUDE_DIR})

# On win32, qt statically links to zlib and libpng, and exports their symbols.
# We can just link to Qt to get zlib and libpng. We still require the user to
# supply their own headers on the search path, but they can and should just
# specify ${QTDIR}/src/3rdparty/zlib/include .
# To use this, you must use FindQt before FindZlib.
IF(QT_QTCORE_LIBRARY AND USE_QT_ZLIB_PNGLIB)

  MESSAGE("Using zlib from qt")
  SET(ZLIB_LIBRARY_RELEASE ${QT_QTCORE_LIBRARY_RELEASE})
  SET(ZLIB_LIBRARY_DEBUG ${QT_QTCORE_LIBRARY_DEBUG})
  SET(ZLIB_LIBRARY ${QT_QTCORE_LIBRARY})

ELSE(QT_QTCORE_LIBRARY AND USE_QT_ZLIB_PNGLIB)

	IF (NOT ZLIB_LIBRARY_DIR)
		set(ZLIB_LIBRARY_DIR)
	ENDIF (NOT ZLIB_LIBRARY_DIR)
	find_library(
		ZLIB_LIBRARY
		NAMES zlib1 zlib zdll z
		PATHS ${ZLIB_LIBRARY_DIR})

ENDIF(QT_QTCORE_LIBRARY AND USE_QT_ZLIB_PNGLIB)

# Handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE)

IF(ZLIB_FOUND)
	MESSAGE(STATUS "Found ZLIB library -- " ${ZLIB_LIBRARY})
ELSE(ZLIB_FOUND)
	MESSAGE(STATUS "Could not find ZLIB library")
ENDIF(ZLIB_FOUND)
