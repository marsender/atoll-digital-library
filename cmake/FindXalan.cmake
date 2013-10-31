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
# - Try to find Xalan library
#
# Once done this will define
#
#  XALAN_FOUND - system has Xalan
#  XALAN_INCLUDE - the Xalan include directory
#  XALAN_INCLUDE_MSG - the Xalan include messages directory (windows only)
#  XALAN_LIBRARY - Link these to use Xalan
#

IF (XALAN_INCLUDE AND XALAN_INCLUDE_MSG AND XALAN_LIBRARY)
	# In cache already
	set(XALAN_FIND_QUIETLY TRUE)
ENDIF (XALAN_INCLUDE AND XALAN_INCLUDE_MSG AND XALAN_LIBRARY)

# Look for the header file
IF (NOT XALAN_INCLUDE_DIR)
	set(XALAN_INCLUDE_DIR)
ENDIF (NOT XALAN_INCLUDE_DIR)
find_path(
	XALAN_INCLUDE
	NAMES xalanc/XPath/XObjectFactory.hpp
	PATHS ${XALAN_INCLUDE_DIR})

# Look for the header message file
IF (NOT XALAN_INCLUDE_MSG_DIR)
	IF (WIN32)
		set(XALAN_INCLUDE_MSG_DIR)
	ELSE()
		set(XALAN_INCLUDE_MSG TRUE)
		set(XALAN_INCLUDE_MSG_DIR "")
	ENDIF()
ENDIF (NOT XALAN_INCLUDE_MSG_DIR)
IF (WIN32)
find_path(
	XALAN_INCLUDE_MSG
	NAMES LocalMsgIndex.hpp
	PATHS ${XALAN_INCLUDE_MSG_DIR})
ENDIF()

# Look for the library
IF (WIN32)
	SET (xalan_libs "Xalan-C_1")
ELSE()
	SET (xalan_libs "xalan-c")
ENDIF()
IF (NOT XALAN_LIBRARY_DIR)
	set(XALAN_LIBRARY_DIR)
ENDIF (NOT XALAN_LIBRARY_DIR)
find_library(
	XALAN_LIBRARY
	NAMES ${xalan_libs}
	PATHS ${XALAN_LIBRARY_DIR})

IF (XALAN_INCLUDE AND XALAN_INCLUDE_MSG AND XALAN_LIBRARY)
	set(XALAN_FOUND TRUE)
ELSE (XALAN_INCLUDE AND XALAN_INCLUDE_MSG AND XALAN_LIBRARY)
	set(XALAN_FOUND FALSE)
ENDIF (XALAN_INCLUDE AND XALAN_INCLUDE_MSG AND XALAN_LIBRARY)

IF (XALAN_FOUND)
	#IF (NOT XALAN_FIND_QUIETLY)
	MESSAGE(STATUS "Found Xalan library -- " ${XALAN_LIBRARY})
	#ENDIF (NOT XALAN_FIND_QUIETLY)
ELSE (XALAN_FOUND)
	MESSAGE(STATUS "Could not find Xalan library")
ENDIF (XALAN_FOUND)

unset(xalan_libs CACHE)
