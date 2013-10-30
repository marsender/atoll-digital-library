cmake_minimum_required(VERSION 2.6)

project(atoll-digital-library)

# Set the default build type to release
# NOTE: the "NMake Makefiles" generator will always set the default build type to "Debug"
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug, Release, RelWithDebInfo and MinSizeRel" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Define vars
set(ATOLL_VERSION_MAJOR "1")
set(ATOLL_VERSION_MINOR "6")
set(ATOLL_VERSION_PATCH "0")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Init directories to search for CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompareVersionStrings)
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

# BDB
message(STATUS "Looking for BDB")
find_package(BDB)
if(BDB_FOUND)
	include_directories(${BDB_INCLUDE})
else()
	MESSAGE(FATAL_ERROR "BDB library is required")
endif()
message(STATUS "")

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

# ZLIB
message(STATUS "Looking for ZLIB")
find_package(ZLIB)
if(ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE})
else()
	message(FATAL_ERROR "Zlib library is required")
endif()
message(STATUS "")

# CppUnit
message(STATUS "Looking for CppUnit")
find_package(CppUnit)
if(CPPUNIT_FOUND)
	include_directories(${CPPUNIT_INCLUDE})
else()
	message(FATAL_ERROR "CppUnit library is required")
endif()
message(STATUS "")

# Vld
if(MSVC)
	message(STATUS "Looking for Vld")
	find_package(Vld)
	if(VLD_FOUND)
		include_directories(${VLD_INCLUDE})
	else()
		message(FATAL_ERROR "Vld library is required")
	endif()
	message(STATUS "")
endif()

# Doxygen documentation
include(Doxygen)

# Write out the configuration
configure_file(${CMAKE_SOURCE_DIR}/cmake/AtollConfig.hpp.cmake ${CMAKE_BINARY_DIR}/Config.hpp)

# Add the project groups
source_group("adorner" ${CMAKE_SOURCE_DIR}/src/adorner/*)
source_group("collection" ${CMAKE_SOURCE_DIR}/src/collection/*)
source_group("common" ${CMAKE_SOURCE_DIR}/src/common/*)
#source_group("console" ${CMAKE_SOURCE_DIR}/src/console/*)
source_group("database" ${CMAKE_SOURCE_DIR}/src/database/*)
source_group("engine" ${CMAKE_SOURCE_DIR}/src/engine/*)
source_group("indexer" ${CMAKE_SOURCE_DIR}/src/indexer/*)
source_group("lucene\\analysis" ${CMAKE_SOURCE_DIR}/src/lucene/analysis/*)
source_group("lucene\\index" ${CMAKE_SOURCE_DIR}/src/lucene/index/*)
source_group("lucene\\queryparser" ${CMAKE_SOURCE_DIR}/src/lucene/queryparser/*)
source_group("lucene\\search" ${CMAKE_SOURCE_DIR}/src/lucene/search/*)
source_group("lucene\\util" ${CMAKE_SOURCE_DIR}/src/lucene/util/*)
source_group("parser" ${CMAKE_SOURCE_DIR}/src/parser/*)
source_group("plugin" ${CMAKE_SOURCE_DIR}/src/plugin/*)
source_group("recordbreaker" ${CMAKE_SOURCE_DIR}/src/recordbreaker/*)
source_group("searcher" ${CMAKE_SOURCE_DIR}/src/searcher/*)
source_group("tokenizer" ${CMAKE_SOURCE_DIR}/src/tokenizer/*)
source_group("unittest" ${CMAKE_SOURCE_DIR}/src/unittest/*)
source_group("util" ${CMAKE_SOURCE_DIR}/src/util/*)

# Atoll console files
set(atoll_console_files
	${CMAKE_SOURCE_DIR}/src/console/Main.cpp
	)

# Atoll engine files
set(atoll_engine_files
	${CMAKE_SOURCE_DIR}/src/adorner/Adorner.cpp
	${CMAKE_SOURCE_DIR}/src/adorner/AdornerConfig.cpp
	${CMAKE_SOURCE_DIR}/src/adorner/AdornerDecorator.cpp

	${CMAKE_SOURCE_DIR}/src/collection/Collection.cpp
	${CMAKE_SOURCE_DIR}/src/collection/ColManager.cpp

	${CMAKE_SOURCE_DIR}/src/common/AppAssert.cpp
	${CMAKE_SOURCE_DIR}/src/common/AppException.cpp
	${CMAKE_SOURCE_DIR}/src/common/BinaryBuffer.cpp
	${CMAKE_SOURCE_DIR}/src/common/Containers.cpp
	${CMAKE_SOURCE_DIR}/src/common/FileSystem.cpp
	${CMAKE_SOURCE_DIR}/src/common/Logger.cpp
	${CMAKE_SOURCE_DIR}/src/common/Md5.cpp
	${CMAKE_SOURCE_DIR}/src/common/StdString.cpp
	${CMAKE_SOURCE_DIR}/src/common/Trace.cpp
	${CMAKE_SOURCE_DIR}/src/common/UnicodeUtil.cpp
	${CMAKE_SOURCE_DIR}/src/common/UnitTest.cpp
	${CMAKE_SOURCE_DIR}/src/common/Util.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesBaseHandler.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesCatalogResolver.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesEntityResolver.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesNode.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesParser.cpp
	${CMAKE_SOURCE_DIR}/src/common/XercesString.cpp
	${CMAKE_SOURCE_DIR}/src/common/Zlib.cpp

	${CMAKE_SOURCE_DIR}/src/database/DbBlob.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbColMeta.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbCursor.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbDoc.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbDocMeta.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbFieldMeta.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbManager.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbMap.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbNdx.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbSearchRecord.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbUtil.cpp
	${CMAKE_SOURCE_DIR}/src/database/DbWrapper.cpp

	${CMAKE_SOURCE_DIR}/src/engine/App.cpp
	${CMAKE_SOURCE_DIR}/src/engine/EngineApi.cpp
	${CMAKE_SOURCE_DIR}/src/engine/EngineEnv.cpp
	${CMAKE_SOURCE_DIR}/src/engine/EngineLib.cpp
	${CMAKE_SOURCE_DIR}/src/engine/InitStringDecl.cpp

	${CMAKE_SOURCE_DIR}/src/indexer/Indexer.cpp
	${CMAKE_SOURCE_DIR}/src/indexer/IndexerConfig.cpp
	${CMAKE_SOURCE_DIR}/src/indexer/IndexerConfigNode.cpp
	${CMAKE_SOURCE_DIR}/src/indexer/IndexerConfigParam.cpp
	${CMAKE_SOURCE_DIR}/src/indexer/IndexerItem.cpp
	${CMAKE_SOURCE_DIR}/src/indexer/IndexerNode.cpp

	${CMAKE_SOURCE_DIR}/src/lucene/analysis/AnalysisHeader.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/analysis/Analyzers.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/analysis/StandardAnalyzer.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/analysis/StandardFilter.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/analysis/StandardTokenizer.cpp

	${CMAKE_SOURCE_DIR}/src/lucene/index/SegmentTermEnum.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/index/Term.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/index/Terms.cpp

	${CMAKE_SOURCE_DIR}/src/lucene/queryparser/Lexer.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/queryparser/QueryParser.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/queryparser/QueryParserBase.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/queryparser/QueryToken.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/queryparser/TokenList.cpp

	${CMAKE_SOURCE_DIR}/src/lucene/search/BooleanClause.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/BooleanQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/FilteredTermEnum.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/FuzzyQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/FuzzyTermEnum.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/MultiTermQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/PhraseQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/PrefixQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/Query.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/RangeQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/TermQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/WildcardQuery.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/search/WildcardTermEnum.cpp

	${CMAKE_SOURCE_DIR}/src/lucene/util/Equators.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/util/FastCharStream.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/util/Misc.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/util/Reader.cpp
	${CMAKE_SOURCE_DIR}/src/lucene/util/StringBuffer.cpp

	${CMAKE_SOURCE_DIR}/src/parser/AdornerHandler.cpp
	${CMAKE_SOURCE_DIR}/src/parser/IndexerConfigHandler.cpp
	${CMAKE_SOURCE_DIR}/src/parser/IndexerHandler.cpp
	${CMAKE_SOURCE_DIR}/src/parser/Parser.cpp
	${CMAKE_SOURCE_DIR}/src/parser/RecordBreakerConfigHandler.cpp

	${CMAKE_SOURCE_DIR}/src/plugin/Plugin.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/PluginKernel.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/PluginMessage.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/PluginServer.cpp

	${CMAKE_SOURCE_DIR}/src/recordbreaker/RecordBreaker.cpp
	${CMAKE_SOURCE_DIR}/src/recordbreaker/RecordBreakerConfig.cpp
	${CMAKE_SOURCE_DIR}/src/recordbreaker/RecordBreakerConfigParam.cpp

	${CMAKE_SOURCE_DIR}/src/searcher/QueryResolver.cpp
	${CMAKE_SOURCE_DIR}/src/searcher/Searcher.cpp

	${CMAKE_SOURCE_DIR}/src/tokenizer/Token.cpp

	${CMAKE_SOURCE_DIR}/src/unittest/AdornerDecoratorTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/AdornerHandlerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/AdornerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/BasicTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/CollectionTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/ColManagerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/ColMetaTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbBlobTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbColMetaTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbDocMetaTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbDocTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbFieldMetaTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbManagerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbMapTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbNdxTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DbSearchRecordTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DocMetaTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/DocRecordTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/EntryTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/EnumStringTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/FileSystemTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/IndexerHandlerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/IndexerItemTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/IndexerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/Md5Test.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/ParserTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/PluginXmlExecTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/PluginXsltTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/QueryParserTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/RecordBreakerTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/SearcherTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/StdStringTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/UnicodeUtilTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/UnitTestSuites.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/XercesTest.cpp
	${CMAKE_SOURCE_DIR}/src/unittest/ZlibTest.cpp

	${CMAKE_SOURCE_DIR}/src/util/ColMeta.cpp
	${CMAKE_SOURCE_DIR}/src/util/DocMeta.cpp
	${CMAKE_SOURCE_DIR}/src/util/DocRecord.cpp
	${CMAKE_SOURCE_DIR}/src/util/Entry.cpp
	${CMAKE_SOURCE_DIR}/src/util/FieldMeta.cpp
	${CMAKE_SOURCE_DIR}/src/util/SearchCrit.cpp
	${CMAKE_SOURCE_DIR}/src/util/SearchRecord.cpp
	${CMAKE_SOURCE_DIR}/src/util/WordEntries.cpp
	${CMAKE_SOURCE_DIR}/src/util/WordEntry.cpp
	${CMAKE_SOURCE_DIR}/src/util/WordInt.cpp
	)

# Atoll plugin xmlexec files
set(atoll_plugin_xmlexec_files
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/PluginXmlExec.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/XmlExecCmd.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/XmlExecHandler.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/XmlExecParser.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/XmlExecRunner.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReq.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqCol.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqDb.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqDoc.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqIndex.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqInfo.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqInit.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xmlexec/core/XmlReqSearch.cpp
	)

# Atoll plugin xslt files
set(atoll_plugin_xslt_files
	${CMAKE_SOURCE_DIR}/src/plugin/xslt/PluginXslt.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xslt/XsltManager.cpp
	${CMAKE_SOURCE_DIR}/src/plugin/xslt/XsltTransformer.cpp
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
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USRDLL /DDEF_LibExport")
	# Adds -D define flags to the compilation of source files
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Build atoll_console
add_executable(atoll_console ${atoll_console_files})
target_link_libraries(atoll_console atoll_engine)

# Build atoll_engine
add_library(atoll_engine SHARED ${atoll_engine_files})
target_link_libraries(atoll_engine ${BDB_LIBRARY} ${XERCESC_LIBRARY} ${ICU_LIBRARY} ${ICU_IO_LIBRARIES} ${ICU_I18N_LIBRARIES} ${ICU_DATA_LIBRARIES} ${CPPUNIT_LIBRARY} ${ZLIB_LIBRARY})
if(MSVC)
	SET_TARGET_PROPERTIES(atoll_engine PROPERTIES COMPILE_DEFINITIONS "${export_definitions}")
	#SET_TARGET_PROPERTIES(atoll_engine PROPERTIES
	#	COMPILE_DEFINITIONS ${export_definitions}
	#	COMPILE_FLAGS "/D_USRDLL /DDEF_LibExport"
	#	LINK_FLAGS "TESTS_EXE_LINKER_FLAGS")
endif()

# Build atoll_plugin_xmlexec
add_library(atoll_plugin_xmlexec SHARED ${atoll_plugin_xmlexec_files})
target_link_libraries(atoll_plugin_xmlexec atoll_engine)
if(MSVC)
	SET_TARGET_PROPERTIES(atoll_plugin_xmlexec PROPERTIES COMPILE_DEFINITIONS "${export_definitions}")
endif()

# Build atoll_plugin_xslt
add_library(atoll_plugin_xslt SHARED ${atoll_plugin_xslt_files})
target_link_libraries(atoll_plugin_xslt atoll_engine ${XALAN_LIBRARY})
if(MSVC)
	SET_TARGET_PROPERTIES(atoll_plugin_xslt PROPERTIES COMPILE_DEFINITIONS "${export_definitions}")
endif()

# Tests
enable_testing()
add_test(NAME test_engine
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND ${CMAKE_BINARY_DIR}/bin/atoll_console -j test -l ./data_unittest/_atoll.log
	)
#set_tests_properties(test_engine PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
#set_tests_properties(test_engine PROPERTIES PASS_REGULAR_EXPRESSION "Passed")
add_test(NAME test_plugin_xmlexec
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND ${CMAKE_BINARY_DIR}/bin/atoll_console -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_test.xml
	)

# Install
set(CMAKE_INSTALL_SO_NO_EXE "0")
install(TARGETS atoll_console atoll_engine atoll_plugin_xmlexec atoll_plugin_xslt
	RUNTIME DESTINATION /usr/bin
	LIBRARY DESTINATION /usr/local/lib
	CONFIGURATIONS Release
	)

# Package
#   http://www.cmake.org/Wiki/CMake:CPackPackageGenerators
#   https://wiki.debian.org/HowToPackageForDebian
SET(CPACK_SET_DESTDIR "ON")
set(CPACK_GENERATOR "TGZ;TBZ2;DEB") # TGZ;TBZ2;DEB
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "${ATOLL_VERSION_MAJOR}.${ATOLL_VERSION_MINOR}.${ATOLL_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Atoll Digital Library is a document publishing system that allows users to create their own digital library.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Didier Corbiere <contact@atoll-digital-library.org>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.atoll-digital-library.org/")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libicu48, libdb4.8, libcppunit (>= 1.12), libxerces-c (>= 2.8.0), libxalan (>= 1.10)")
# Set the displayed names for each of the components to install.
# These will be displayed in the list of components inside the installer.
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Atoll Digital Library")
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Atoll libraries")
include(CPack)
