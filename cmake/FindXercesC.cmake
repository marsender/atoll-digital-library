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
# - Try to find Xerces-C library
#
# Once done this will define
#
#  XERCESC_FOUND - system has Xerces-C
#  XERCESC_INCLUDE - the Xerces-C include directory
#  XERCESC_LIBRARY - Link these to use Xerces-C
#  XERCESC_VERSION - Xerces-C found version
#

IF (XERCESC_INCLUDE AND XERCESC_LIBRARY)
	# In cache already
	set(XERCESC_FIND_QUIETLY TRUE)
ENDIF (XERCESC_INCLUDE AND XERCESC_LIBRARY)

# Look for the header file
IF (NOT XERCESC_INCLUDE_DIR)
	set(XERCESC_INCLUDE_DIR)
ENDIF (NOT XERCESC_INCLUDE_DIR)
find_path(
	XERCESC_INCLUDE
	NAMES xercesc/util/XercesVersion.hpp
	PATHS ${XERCESC_INCLUDE_DIR})

# Look for the library
IF (NOT XERCESC_LIBRARY_DIR)
	set(XERCESC_LIBRARY_DIR)
ENDIF (NOT XERCESC_LIBRARY_DIR)
find_library(
	XERCESC_LIBRARY
	NAMES xerces-c xerces-c_2 xerces-c_3
	PATHS ${XERCESC_LIBRARY_DIR})

IF (XERCESC_INCLUDE AND XERCESC_LIBRARY)
	set(XERCESC_FOUND TRUE)
ELSE (XERCESC_INCLUDE AND XERCESC_LIBRARY)
	set(XERCESC_FOUND FALSE)
ENDIF (XERCESC_INCLUDE AND XERCESC_LIBRARY)

IF (XERCESC_FOUND)
	#IF (NOT XERCESC_FIND_QUIETLY)
	MESSAGE(STATUS "Found Xerces-C library -- " ${XERCESC_LIBRARY})
	#ENDIF (NOT XERCESC_FIND_QUIETLY)
ELSE (XERCESC_FOUND)
	MESSAGE(STATUS "Could not find Xerces-C library")
ENDIF (XERCESC_FOUND)

# Look for Xerces-C version
find_path(
	XVERHPPPATH
	NAMES XercesVersion.hpp
	PATHS ${XERCESC_INCLUDE}/xercesc/util
	NO_DEFAULT_PATH)
IF (${XVERHPPPATH} STREQUAL XVERHPPPATH-NOTFOUND)
  set(XERCES_VERSION "0")
ELSE (${XVERHPPPATH} STREQUAL XVERHPPPATH-NOTFOUND)
  FILE(READ ${XVERHPPPATH}/XercesVersion.hpp XVERHPP)
  STRING(REGEX MATCH "\n *#define XERCES_VERSION_MAJOR +[0-9]+" XVERMAJ ${XVERHPP})
  STRING(REGEX MATCH "\n *#define XERCES_VERSION_MINOR +[0-9]+" XVERMIN ${XVERHPP})
  STRING(REGEX MATCH "\n *#define XERCES_VERSION_REVISION +[0-9]+" XVERREV ${XVERHPP})
  STRING(REGEX REPLACE "\n *#define XERCES_VERSION_MAJOR +" "" XVERMAJ ${XVERMAJ})
  STRING(REGEX REPLACE "\n *#define XERCES_VERSION_MINOR +" "" XVERMIN ${XVERMIN})
  STRING(REGEX REPLACE "\n *#define XERCES_VERSION_REVISION +" "" XVERREV ${XVERREV})
  set(XERCESC_VERSION ${XVERMAJ}.${XVERMIN}.${XVERREV})
  #MESSAGE(STATUS "Xerces-C Version: ${XERCESC_VERSION}")
ENDIF (${XVERHPPPATH} STREQUAL XVERHPPPATH-NOTFOUND)
unset(XVERHPPPATH CACHE)
