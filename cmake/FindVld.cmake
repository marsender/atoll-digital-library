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
# - Try to find Vld library
#
# Once done this will define
#
#  VLD_FOUND   - system has Vld
#  VLD_INCLUDE - the Vld include directory
#  VLD_LIBRARY - Link these to use Vld
#

IF (VLD_INCLUDE AND VLD_LIBRARY)
	# In cache already
	set(VLD_FIND_QUIETLY TRUE)
ENDIF (VLD_INCLUDE AND VLD_LIBRARY)

# Look for the header file
IF (NOT VLD_INCLUDE_DIR)
	set(VLD_INCLUDE_DIR)
ENDIF (NOT VLD_INCLUDE_DIR)
find_path(
	VLD_INCLUDE
	NAMES vld.h
	PATHS ${VLD_INCLUDE_DIR})

IF (NOT VLD_LIBRARY_DIR)
	set(VLD_LIBRARY_DIR)
ENDIF (NOT VLD_LIBRARY_DIR)
find_library(
	VLD_LIBRARY
	NAMES vld.lib
	PATHS ${VLD_LIBRARY_DIR})

IF (VLD_INCLUDE AND VLD_LIBRARY)
	set(VLD_FOUND TRUE)
ELSE (VLD_INCLUDE AND VLD_LIBRARY)
	set(VLD_FOUND FALSE)
ENDIF (VLD_INCLUDE AND VLD_LIBRARY)

IF (VLD_FOUND)
	MESSAGE(STATUS "Found Vld library -- " ${VLD_LIBRARY})
ELSE (VLD_FOUND)
	MESSAGE(STATUS "Could not find Vld library")
ENDIF (VLD_FOUND)
