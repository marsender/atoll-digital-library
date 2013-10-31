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
# - Try to find Berkley db library
#
# Once done this will define
#
#  BDB_FOUND     - True if BDB found.
#  BDB_INCLUDE   - Directory to include to get BDB headers
#  BDB_LIBRARY   - Libraries to link
#

# Look for the header file
IF (NOT BDB_INCLUDE_DIR)
	set(BDB_INCLUDE_DIR)
ENDIF (NOT BDB_INCLUDE_DIR)
find_path(
  BDB_INCLUDE
  NAMES db_cxx.h
  PATHS ${BDB_INCLUDE_DIR}
  DOC "Include directory for the BDB library")

# Look for the library
IF (NOT BDB_LIBRARY_DIR)
	set(BDB_LIBRARY_DIR)
ENDIF (NOT BDB_LIBRARY_DIR)
IF (NOT BDB_VERSION)
	set(BDB_VERSION)
ENDIF (NOT BDB_VERSION)
find_library(
  BDB_LIBRARY
  NAMES db_cxx libdb${BDB_VERSION}
  PATHS ${BDB_LIBRARY_DIR}
  DOC "Libraries to link against for the common parts of BDB")

# Copy the results to the output variables
if(BDB_INCLUDE AND BDB_LIBRARY)
  set(BDB_FOUND 1)
else(BDB_INCLUDE AND BDB_LIBRARY)
  set(BDB_FOUND 0)
endif(BDB_INCLUDE AND BDB_LIBRARY)

if (BDB_FOUND)
  MESSAGE(STATUS "Found BDB library -- " ${BDB_LIBRARY})
else (BDB_FOUND)
  MESSAGE(STATUS "Could not find BDB library")
endif (BDB_FOUND)
