#
# Copyright © 2010 Didier Corbiere
#
# Distributable under the terms of the GNU Lesser General Public License,
# as specified in the LICENSE file.
#
# Makefile atoll
#
# This makefile is an alternative to cmake
#
# CXXFLAGS and CFLAGS may have -DDEF_EnableAsserts
# zlib link: -lz
#

PLATFORM = LINUX
COMPILER = g++
CXXVER =
GCC = yes
GXX = yes
CXXFLAGS = -Wall -O2 -DNDEBUG -DXML_USE_PTHREADS
CFLAGS = -Wall -O2 -DNDEBUG -DXML_USE_PTHREADS
LDFLAGS =
THREADS = pthread
EXTRA_LIBS = -lpthread
PREFIX = /usr/local

################## LIBRARY NAMES AND DIRECTORIES ##################

INCLUDES= -I. -I/usr/local/include
ENGINE_INCLUDES= ${INCLUDES}
ENGINE_LIBRARY_NAMES=-lz -ldl -ldb_cxx -lcppunit -licudata -licui18n -licuio -licuuc -lxerces-c
PLUGIN_XMLEXEC_LIBRARY_NAMES=-ldl -lcppunit -licudata -licui18n -licuio -licuuc -lxerces-c
PLUGIN_XSLT_LIBRARY_NAMES=-ldl -lcppunit -licudata -licui18n -licuio -licuuc -lxerces-c -lxalan-c

SRCDIR= ../../../src
INSTALLDIR= /usr/local/lib
BININSTALLDIR= /usr/bin
BINOUTDIR= ./out/atoll/bin

####################### PLATFORM DEPENDENCIES #####################
CMP= -c ${CXXFLAGS}
CC= ${COMPILER} -c -D${PLATFORM} -fpic
LINK = ${COMPILER} -D${PLATFORM} -fpic ${LDFLAGS}
PLATFORM_LIB_LINK_OPTIONS=-L/usr/local/lib
EXTRA_LINK_OPTIONS=${EXTRA_LIBS}

ifneq (${THREADS},none)
	CMP += -D_REENTRANT
endif

################## PROGRAM ##################

CONSOLE_NAME=atoll_console
CONSOLE_OUTDIR= ${BINOUTDIR}/obj/${CONSOLE_NAME}
CONSOLE_SRCDIR= ${SRCDIR}
CONSOLE_OBJS= \
	${CONSOLE_OUTDIR}/Main.o \

PLUGIN_XMLEXEC_NAME=atoll_plugin_xmlexec
PLUGIN_XMLEXEC_LIBNAME=lib${PLUGIN_XMLEXEC_NAME}.so
PLUGIN_XMLEXEC_OUTDIR= ${BINOUTDIR}/obj/${PLUGIN_XMLEXEC_NAME}
PLUGIN_XMLEXEC_SRCDIR= ${SRCDIR}
PLUGIN_XMLEXEC_OBJS= \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReq.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqCol.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDb.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDoc.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqIndex.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInfo.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInit.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlReqSearch.o \
	\
	${PLUGIN_XMLEXEC_OUTDIR}/PluginXmlExec.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlExecCmd.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlExecHandler.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlExecParser.o \
	${PLUGIN_XMLEXEC_OUTDIR}/XmlExecRunner.o \

PLUGIN_XSLT_NAME=atoll_plugin_xslt
PLUGIN_XSLT_LIBNAME=lib${PLUGIN_XSLT_NAME}.so
PLUGIN_XSLT_OUTDIR= ${BINOUTDIR}/obj/${PLUGIN_XSLT_NAME}
PLUGIN_XSLT_SRCDIR= ${SRCDIR}
PLUGIN_XSLT_OBJS= \
	${PLUGIN_XSLT_OUTDIR}/PluginXslt.o \
	${PLUGIN_XSLT_OUTDIR}/XsltManager.o \
	${PLUGIN_XSLT_OUTDIR}/XsltTransformer.o \

ENGINE_NAME=atoll_engine
ENGINE_LIBNAME=lib${ENGINE_NAME}.so
ENGINE_OUTDIR= ${BINOUTDIR}/obj/${ENGINE_NAME}
ENGINE_SRCDIR= ${SRCDIR}
ENGINE_OBJS= \
	${ENGINE_OUTDIR}/Adorner.o \
	${ENGINE_OUTDIR}/AdornerConfig.o \
	${ENGINE_OUTDIR}/AdornerDecorator.o \
	\
	${ENGINE_OUTDIR}/Collection.o \
	${ENGINE_OUTDIR}/ColManager.o \
	\
	${ENGINE_OUTDIR}/AppAssert.o \
	${ENGINE_OUTDIR}/AppException.o \
	${ENGINE_OUTDIR}/BinaryBuffer.o \
	${ENGINE_OUTDIR}/Containers.o \
	${ENGINE_OUTDIR}/FileSystem.o \
	${ENGINE_OUTDIR}/Logger.o \
	${ENGINE_OUTDIR}/Md5.o \
	${ENGINE_OUTDIR}/StdString.o \
	${ENGINE_OUTDIR}/Trace.o \
	${ENGINE_OUTDIR}/UnicodeUtil.o \
	${ENGINE_OUTDIR}/UnitTest.o \
	${ENGINE_OUTDIR}/Util.o \
	${ENGINE_OUTDIR}/XercesBaseHandler.o \
	${ENGINE_OUTDIR}/XercesCatalogResolver.o \
	${ENGINE_OUTDIR}/XercesEntityResolver.o \
	${ENGINE_OUTDIR}/XercesNode.o \
	${ENGINE_OUTDIR}/XercesParser.o \
	${ENGINE_OUTDIR}/XercesString.o \
	${ENGINE_OUTDIR}/Zlib.o \
	\
	${ENGINE_OUTDIR}/DbBlob.o \
	${ENGINE_OUTDIR}/DbColMeta.o \
	${ENGINE_OUTDIR}/DbCursor.o \
	${ENGINE_OUTDIR}/DbDoc.o \
	${ENGINE_OUTDIR}/DbDocMeta.o \
	${ENGINE_OUTDIR}/DbFieldMeta.o \
	${ENGINE_OUTDIR}/DbManager.o \
	${ENGINE_OUTDIR}/DbMap.o \
	${ENGINE_OUTDIR}/DbNdx.o \
	${ENGINE_OUTDIR}/DbSearchRecord.o \
	${ENGINE_OUTDIR}/DbUtil.o \
	${ENGINE_OUTDIR}/DbWrapper.o \
	\
	${ENGINE_OUTDIR}/App.o \
	${ENGINE_OUTDIR}/EngineApi.o \
	${ENGINE_OUTDIR}/EngineEnv.o \
	${ENGINE_OUTDIR}/EngineLib.o \
	${ENGINE_OUTDIR}/InitStringDecl.o \
	\
	${ENGINE_OUTDIR}/Indexer.o \
	${ENGINE_OUTDIR}/IndexerConfig.o \
	${ENGINE_OUTDIR}/IndexerConfigNode.o \
	${ENGINE_OUTDIR}/IndexerConfigParam.o \
	${ENGINE_OUTDIR}/IndexerItem.o \
	${ENGINE_OUTDIR}/IndexerNode.o \
	\
	${ENGINE_OUTDIR}/AnalysisHeader.o \
	${ENGINE_OUTDIR}/Analyzers.o \
	${ENGINE_OUTDIR}/StandardAnalyzer.o \
	${ENGINE_OUTDIR}/StandardFilter.o \
	${ENGINE_OUTDIR}/StandardTokenizer.o \
	\
	${ENGINE_OUTDIR}/SegmentTermEnum.o \
	${ENGINE_OUTDIR}/Term.o \
	${ENGINE_OUTDIR}/Terms.o \
	\
	${ENGINE_OUTDIR}/Lexer.o \
	${ENGINE_OUTDIR}/QueryParser.o \
	${ENGINE_OUTDIR}/QueryParserBase.o \
	${ENGINE_OUTDIR}/QueryToken.o \
	${ENGINE_OUTDIR}/TokenList.o \
	\
	${ENGINE_OUTDIR}/BooleanClause.o \
	${ENGINE_OUTDIR}/BooleanQuery.o \
	${ENGINE_OUTDIR}/FilteredTermEnum.o \
	${ENGINE_OUTDIR}/FuzzyQuery.o \
	${ENGINE_OUTDIR}/FuzzyTermEnum.o \
	${ENGINE_OUTDIR}/MultiTermQuery.o \
	${ENGINE_OUTDIR}/PhraseQuery.o \
	${ENGINE_OUTDIR}/PrefixQuery.o \
	${ENGINE_OUTDIR}/Query.o \
	${ENGINE_OUTDIR}/RangeQuery.o \
	${ENGINE_OUTDIR}/TermQuery.o \
	${ENGINE_OUTDIR}/WildcardQuery.o \
	${ENGINE_OUTDIR}/WildcardTermEnum.o \
	\
	${ENGINE_OUTDIR}/Equators.o \
	${ENGINE_OUTDIR}/FastCharStream.o \
	${ENGINE_OUTDIR}/Misc.o \
	${ENGINE_OUTDIR}/Reader.o \
	${ENGINE_OUTDIR}/StringBuffer.o \
	\
	${ENGINE_OUTDIR}/AdornerHandler.o \
	${ENGINE_OUTDIR}/IndexerConfigHandler.o \
	${ENGINE_OUTDIR}/IndexerHandler.o \
	${ENGINE_OUTDIR}/Parser.o \
	${ENGINE_OUTDIR}/RecordBreakerConfigHandler.o \
	\
	${ENGINE_OUTDIR}/Plugin.o \
	${ENGINE_OUTDIR}/PluginKernel.o \
	${ENGINE_OUTDIR}/PluginMessage.o \
	${ENGINE_OUTDIR}/PluginServer.o \
	\
	${ENGINE_OUTDIR}/RecordBreaker.o \
	${ENGINE_OUTDIR}/RecordBreakerConfig.o \
	${ENGINE_OUTDIR}/RecordBreakerConfigParam.o \
	\
	${ENGINE_OUTDIR}/QueryResolver.o \
	${ENGINE_OUTDIR}/Searcher.o \
	\
	${ENGINE_OUTDIR}/Token.o \
	\
	${ENGINE_OUTDIR}/AdornerDecoratorTest.o \
	${ENGINE_OUTDIR}/AdornerHandlerTest.o \
	${ENGINE_OUTDIR}/AdornerTest.o \
	${ENGINE_OUTDIR}/BasicTest.o \
	${ENGINE_OUTDIR}/CollectionTest.o \
	${ENGINE_OUTDIR}/ColManagerTest.o \
	${ENGINE_OUTDIR}/ColMetaTest.o \
	${ENGINE_OUTDIR}/DbBlobTest.o \
	${ENGINE_OUTDIR}/DbColMetaTest.o \
	${ENGINE_OUTDIR}/DbDocMetaTest.o \
	${ENGINE_OUTDIR}/DbDocTest.o \
	${ENGINE_OUTDIR}/DbFieldMetaTest.o \
	${ENGINE_OUTDIR}/DbManagerTest.o \
	${ENGINE_OUTDIR}/DbMapTest.o \
	${ENGINE_OUTDIR}/DbNdxTest.o \
	${ENGINE_OUTDIR}/DbSearchRecordTest.o \
	${ENGINE_OUTDIR}/DocMetaTest.o \
	${ENGINE_OUTDIR}/DocRecordTest.o \
	${ENGINE_OUTDIR}/EntryTest.o \
	${ENGINE_OUTDIR}/EnumStringTest.o \
	${ENGINE_OUTDIR}/FileSystemTest.o \
	${ENGINE_OUTDIR}/IndexerHandlerTest.o \
	${ENGINE_OUTDIR}/IndexerItemTest.o \
	${ENGINE_OUTDIR}/IndexerTest.o \
	${ENGINE_OUTDIR}/Md5Test.o \
	${ENGINE_OUTDIR}/ParserTest.o \
	${ENGINE_OUTDIR}/PluginXmlExecTest.o \
	${ENGINE_OUTDIR}/PluginXsltTest.o \
	${ENGINE_OUTDIR}/QueryParserTest.o \
	${ENGINE_OUTDIR}/RecordBreakerTest.o \
	${ENGINE_OUTDIR}/SearcherTest.o \
	${ENGINE_OUTDIR}/StdStringTest.o \
	${ENGINE_OUTDIR}/UnicodeUtilTest.o \
	${ENGINE_OUTDIR}/UnitTestSuites.o \
	${ENGINE_OUTDIR}/XercesTest.o \
	${ENGINE_OUTDIR}/ZlibTest.o \
	\
	${ENGINE_OUTDIR}/ColMeta.o \
	${ENGINE_OUTDIR}/DocMeta.o \
	${ENGINE_OUTDIR}/DocRecord.o \
	${ENGINE_OUTDIR}/Entry.o \
	${ENGINE_OUTDIR}/FieldMeta.o \
	${ENGINE_OUTDIR}/SearchCrit.o \
	${ENGINE_OUTDIR}/SearchRecord.o \
	${ENGINE_OUTDIR}/WordEntries.o \
	${ENGINE_OUTDIR}/WordEntry.o \
	${ENGINE_OUTDIR}/WordInt.o \

all: makedir atoll_engine atoll_plugin_xmlexec atoll_plugin_xslt atoll_console

makedir:
	-mkdir -p ${ENGINE_OUTDIR}
	-mkdir -p ${CONSOLE_OUTDIR}
	-mkdir -p ${PLUGIN_XMLEXEC_OUTDIR}
	-mkdir -p ${PLUGIN_XSLT_OUTDIR}

update:
	cd ../../../src; git pull

install:
	sudo rm -f ${INSTALLDIR}/${ENGINE_LIBNAME}
	sudo cp ${BINOUTDIR}/${ENGINE_LIBNAME} ${INSTALLDIR}
	sudo rm -f ${INSTALLDIR}/${PLUGIN_XMLEXEC_LIBNAME}
	sudo cp ${BINOUTDIR}/${PLUGIN_XMLEXEC_LIBNAME} ${INSTALLDIR}
	sudo rm -f ${INSTALLDIR}/${PLUGIN_XSLT_LIBNAME}
	sudo cp ${BINOUTDIR}/${PLUGIN_XSLT_LIBNAME} ${INSTALLDIR}
	sudo rm -f ${BININSTALLDIR}/${CONSOLE_NAME}
	sudo cp ${BINOUTDIR}/${CONSOLE_NAME} ${BININSTALLDIR}
	sudo apache2ctl stop; sleep 1; sudo ldconfig; sudo apache2ctl start

test:
	cd ..; out/atoll/bin/atoll_console -j test -l ./data_unittest/_atoll.log

clean:
	rm -f ${ENGINE_OBJS} ${BINOUTDIR}/${ENGINE_LIBNAME}
	rm -f ${CONSOLE_OBJS} ${BINOUTDIR}/${CONSOLE_NAME}
	rm -f ${PLUGIN_XMLEXEC_OBJS} ${BINOUTDIR}/${PLUGIN_XMLEXEC_NAME}
	rm -f ${PLUGIN_XSLT_OBJS} ${BINOUTDIR}/${PLUGIN_XSLT_NAME}

atoll_console: ${CONSOLE_OBJS}
	${LINK} ${PLATFORM_LIB_LINK_OPTIONS} ${CONSOLE_OBJS} -o ${BINOUTDIR}/$@ ${EXTRA_LINK_OPTIONS} -l${ENGINE_NAME}

atoll_plugin_xmlexec: ${PLUGIN_XMLEXEC_OBJS}
	${LINK} -shared ${PLATFORM_LIB_LINK_OPTIONS} ${PLUGIN_XMLEXEC_OBJS} -o ${BINOUTDIR}/${PLUGIN_XMLEXEC_LIBNAME} ${PLUGIN_XMLEXEC_LIBRARY_NAMES} ${EXTRA_LINK_OPTIONS} -l${ENGINE_NAME}

atoll_plugin_xslt: ${PLUGIN_XSLT_OBJS}
	${LINK} -shared ${PLATFORM_LIB_LINK_OPTIONS} ${PLUGIN_XSLT_OBJS} -o ${BINOUTDIR}/${PLUGIN_XSLT_LIBNAME} ${PLUGIN_XSLT_LIBRARY_NAMES} ${EXTRA_LINK_OPTIONS} -l${ENGINE_NAME}

atoll_engine: ${ENGINE_OBJS}
	${LINK} -shared ${PLATFORM_LIB_LINK_OPTIONS} ${ENGINE_OBJS} -o ${BINOUTDIR}/${ENGINE_LIBNAME} ${ENGINE_LIBRARY_NAMES} ${EXTRA_LINK_OPTIONS}

# *** console ***

# console
${CONSOLE_OUTDIR}/Main.o:
	${CC} ${INCLUDES} ${CMP} -o ${CONSOLE_OUTDIR}/Main.o ${CONSOLE_SRCDIR}/console/Main.cpp

# *** engine ***

# engine_adorner
${ENGINE_OUTDIR}/Adorner.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Adorner.o ${ENGINE_SRCDIR}/adorner/Adorner.cpp
${ENGINE_OUTDIR}/AdornerConfig.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerConfig.o ${ENGINE_SRCDIR}/adorner/AdornerConfig.cpp
${ENGINE_OUTDIR}/AdornerDecorator.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerDecorator.o ${ENGINE_SRCDIR}/adorner/AdornerDecorator.cpp

# engine_collection
${ENGINE_OUTDIR}/Collection.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Collection.o ${ENGINE_SRCDIR}/collection/Collection.cpp
${ENGINE_OUTDIR}/ColManager.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ColManager.o ${ENGINE_SRCDIR}/collection/ColManager.cpp

# engine_common
${ENGINE_OUTDIR}/AppAssert.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AppAssert.o ${ENGINE_SRCDIR}/common/AppAssert.cpp
${ENGINE_OUTDIR}/AppException.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AppException.o ${ENGINE_SRCDIR}/common/AppException.cpp
${ENGINE_OUTDIR}/BinaryBuffer.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/BinaryBuffer.o ${ENGINE_SRCDIR}/common/BinaryBuffer.cpp
${ENGINE_OUTDIR}/Containers.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Containers.o ${ENGINE_SRCDIR}/common/Containers.cpp
${ENGINE_OUTDIR}/FileSystem.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FileSystem.o ${ENGINE_SRCDIR}/common/FileSystem.cpp
${ENGINE_OUTDIR}/Logger.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Logger.o ${ENGINE_SRCDIR}/common/Logger.cpp
${ENGINE_OUTDIR}/Md5.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Md5.o ${ENGINE_SRCDIR}/common/Md5.cpp
${ENGINE_OUTDIR}/StdString.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StdString.o ${ENGINE_SRCDIR}/common/StdString.cpp
${ENGINE_OUTDIR}/Trace.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Trace.o ${ENGINE_SRCDIR}/common/Trace.cpp
${ENGINE_OUTDIR}/UnicodeUtil.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/UnicodeUtil.o ${ENGINE_SRCDIR}/common/UnicodeUtil.cpp
${ENGINE_OUTDIR}/UnitTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/UnitTest.o ${ENGINE_SRCDIR}/common/UnitTest.cpp
${ENGINE_OUTDIR}/Util.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Util.o ${ENGINE_SRCDIR}/common/Util.cpp
${ENGINE_OUTDIR}/XercesBaseHandler.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesBaseHandler.o ${ENGINE_SRCDIR}/common/XercesBaseHandler.cpp
${ENGINE_OUTDIR}/XercesCatalogResolver.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesCatalogResolver.o ${ENGINE_SRCDIR}/common/XercesCatalogResolver.cpp
${ENGINE_OUTDIR}/XercesEntityResolver.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesEntityResolver.o ${ENGINE_SRCDIR}/common/XercesEntityResolver.cpp
${ENGINE_OUTDIR}/XercesNode.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesNode.o ${ENGINE_SRCDIR}/common/XercesNode.cpp
${ENGINE_OUTDIR}/XercesParser.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesParser.o ${ENGINE_SRCDIR}/common/XercesParser.cpp
${ENGINE_OUTDIR}/XercesString.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesString.o ${ENGINE_SRCDIR}/common/XercesString.cpp
${ENGINE_OUTDIR}/Zlib.o:
	${CC} ${INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Zlib.o ${ENGINE_SRCDIR}/common/Zlib.cpp

# engine_database
${ENGINE_OUTDIR}/DbBlob.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbBlob.o ${ENGINE_SRCDIR}/database/DbBlob.cpp
${ENGINE_OUTDIR}/DbColMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbColMeta.o ${ENGINE_SRCDIR}/database/DbColMeta.cpp
${ENGINE_OUTDIR}/DbCursor.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbCursor.o ${ENGINE_SRCDIR}/database/DbCursor.cpp
${ENGINE_OUTDIR}/DbDoc.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbDoc.o ${ENGINE_SRCDIR}/database/DbDoc.cpp
${ENGINE_OUTDIR}/DbDocMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbDocMeta.o ${ENGINE_SRCDIR}/database/DbDocMeta.cpp
${ENGINE_OUTDIR}/DbFieldMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbFieldMeta.o ${ENGINE_SRCDIR}/database/DbFieldMeta.cpp
${ENGINE_OUTDIR}/DbManager.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbManager.o ${ENGINE_SRCDIR}/database/DbManager.cpp
${ENGINE_OUTDIR}/DbMap.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbMap.o ${ENGINE_SRCDIR}/database/DbMap.cpp
${ENGINE_OUTDIR}/DbNdx.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbNdx.o ${ENGINE_SRCDIR}/database/DbNdx.cpp
${ENGINE_OUTDIR}/DbSearchRecord.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbSearchRecord.o ${ENGINE_SRCDIR}/database/DbSearchRecord.cpp
${ENGINE_OUTDIR}/DbUtil.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbUtil.o ${ENGINE_SRCDIR}/database/DbUtil.cpp
${ENGINE_OUTDIR}/DbWrapper.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbWrapper.o ${ENGINE_SRCDIR}/database/DbWrapper.cpp

# engine
${ENGINE_OUTDIR}/App.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/App.o ${ENGINE_SRCDIR}/engine/App.cpp
${ENGINE_OUTDIR}/EngineApi.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/EngineApi.o ${ENGINE_SRCDIR}/engine/EngineApi.cpp
${ENGINE_OUTDIR}/EngineEnv.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/EngineEnv.o ${ENGINE_SRCDIR}/engine/EngineEnv.cpp
${ENGINE_OUTDIR}/EngineLib.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/EngineLib.o ${ENGINE_SRCDIR}/engine/EngineLib.cpp
${ENGINE_OUTDIR}/InitStringDecl.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/InitStringDecl.o ${ENGINE_SRCDIR}/engine/InitStringDecl.cpp

# engine_indexer
${ENGINE_OUTDIR}/Indexer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Indexer.o ${ENGINE_SRCDIR}/indexer/Indexer.cpp
${ENGINE_OUTDIR}/IndexerConfig.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerConfig.o ${ENGINE_SRCDIR}/indexer/IndexerConfig.cpp
${ENGINE_OUTDIR}/IndexerConfigNode.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerConfigNode.o ${ENGINE_SRCDIR}/indexer/IndexerConfigNode.cpp
${ENGINE_OUTDIR}/IndexerConfigParam.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerConfigParam.o ${ENGINE_SRCDIR}/indexer/IndexerConfigParam.cpp
${ENGINE_OUTDIR}/IndexerItem.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerItem.o ${ENGINE_SRCDIR}/indexer/IndexerItem.cpp
${ENGINE_OUTDIR}/IndexerNode.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerNode.o ${ENGINE_SRCDIR}/indexer/IndexerNode.cpp

# engine_lucene_analysis
${ENGINE_OUTDIR}/AnalysisHeader.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AnalysisHeader.o ${ENGINE_SRCDIR}/lucene/analysis/AnalysisHeader.cpp
${ENGINE_OUTDIR}/Analyzers.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Analyzers.o ${ENGINE_SRCDIR}/lucene/analysis/Analyzers.cpp
${ENGINE_OUTDIR}/StandardAnalyzer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StandardAnalyzer.o ${ENGINE_SRCDIR}/lucene/analysis/StandardAnalyzer.cpp
${ENGINE_OUTDIR}/StandardFilter.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StandardFilter.o ${ENGINE_SRCDIR}/lucene/analysis/StandardFilter.cpp
${ENGINE_OUTDIR}/StandardTokenizer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StandardTokenizer.o ${ENGINE_SRCDIR}/lucene/analysis/StandardTokenizer.cpp

# engine_lucene_index
${ENGINE_OUTDIR}/SegmentTermEnum.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/SegmentTermEnum.o ${ENGINE_SRCDIR}/lucene/index/SegmentTermEnum.cpp
${ENGINE_OUTDIR}/Term.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Term.o ${ENGINE_SRCDIR}/lucene/index/Term.cpp
${ENGINE_OUTDIR}/Terms.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Terms.o ${ENGINE_SRCDIR}/lucene/index/Terms.cpp

# engine_lucene_queryparser
${ENGINE_OUTDIR}/Lexer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Lexer.o ${ENGINE_SRCDIR}/lucene/queryparser/Lexer.cpp
${ENGINE_OUTDIR}/QueryParser.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/QueryParser.o ${ENGINE_SRCDIR}/lucene/queryparser/QueryParser.cpp
${ENGINE_OUTDIR}/QueryParserBase.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/QueryParserBase.o ${ENGINE_SRCDIR}/lucene/queryparser/QueryParserBase.cpp
${ENGINE_OUTDIR}/QueryToken.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/QueryToken.o ${ENGINE_SRCDIR}/lucene/queryparser/QueryToken.cpp
${ENGINE_OUTDIR}/TokenList.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/TokenList.o ${ENGINE_SRCDIR}/lucene/queryparser/TokenList.cpp

# engine_lucene_search
${ENGINE_OUTDIR}/BooleanClause.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/BooleanClause.o ${ENGINE_SRCDIR}/lucene/search/BooleanClause.cpp
${ENGINE_OUTDIR}/BooleanQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/BooleanQuery.o ${ENGINE_SRCDIR}/lucene/search/BooleanQuery.cpp
${ENGINE_OUTDIR}/FilteredTermEnum.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FilteredTermEnum.o ${ENGINE_SRCDIR}/lucene/search/FilteredTermEnum.cpp
${ENGINE_OUTDIR}/FuzzyQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FuzzyQuery.o ${ENGINE_SRCDIR}/lucene/search/FuzzyQuery.cpp
${ENGINE_OUTDIR}/FuzzyTermEnum.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FuzzyTermEnum.o ${ENGINE_SRCDIR}/lucene/search/FuzzyTermEnum.cpp
${ENGINE_OUTDIR}/MultiTermQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/MultiTermQuery.o ${ENGINE_SRCDIR}/lucene/search/MultiTermQuery.cpp
${ENGINE_OUTDIR}/PhraseQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PhraseQuery.o ${ENGINE_SRCDIR}/lucene/search/PhraseQuery.cpp
${ENGINE_OUTDIR}/PrefixQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PrefixQuery.o ${ENGINE_SRCDIR}/lucene/search/PrefixQuery.cpp
${ENGINE_OUTDIR}/Query.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Query.o ${ENGINE_SRCDIR}/lucene/search/Query.cpp
${ENGINE_OUTDIR}/RangeQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RangeQuery.o ${ENGINE_SRCDIR}/lucene/search/RangeQuery.cpp
${ENGINE_OUTDIR}/TermQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/TermQuery.o ${ENGINE_SRCDIR}/lucene/search/TermQuery.cpp
${ENGINE_OUTDIR}/WildcardQuery.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/WildcardQuery.o ${ENGINE_SRCDIR}/lucene/search/WildcardQuery.cpp
${ENGINE_OUTDIR}/WildcardTermEnum.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/WildcardTermEnum.o ${ENGINE_SRCDIR}/lucene/search/WildcardTermEnum.cpp

# engine_lucene_util
${ENGINE_OUTDIR}/Equators.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Equators.o ${ENGINE_SRCDIR}/lucene/util/Equators.cpp
${ENGINE_OUTDIR}/FastCharStream.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FastCharStream.o ${ENGINE_SRCDIR}/lucene/util/FastCharStream.cpp
${ENGINE_OUTDIR}/Misc.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Misc.o ${ENGINE_SRCDIR}/lucene/util/Misc.cpp
${ENGINE_OUTDIR}/Reader.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Reader.o ${ENGINE_SRCDIR}/lucene/util/Reader.cpp
${ENGINE_OUTDIR}/StringBuffer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StringBuffer.o ${ENGINE_SRCDIR}/lucene/util/StringBuffer.cpp

# engine_parser
${ENGINE_OUTDIR}/AdornerHandler.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerHandler.o ${ENGINE_SRCDIR}/parser/AdornerHandler.cpp
${ENGINE_OUTDIR}/IndexerConfigHandler.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerConfigHandler.o ${ENGINE_SRCDIR}/parser/IndexerConfigHandler.cpp
${ENGINE_OUTDIR}/IndexerHandler.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerHandler.o ${ENGINE_SRCDIR}/parser/IndexerHandler.cpp
${ENGINE_OUTDIR}/Parser.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Parser.o ${ENGINE_SRCDIR}/parser/Parser.cpp
${ENGINE_OUTDIR}/RecordBreakerConfigHandler.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RecordBreakerConfigHandler.o ${ENGINE_SRCDIR}/parser/RecordBreakerConfigHandler.cpp

# engine_plugin
${ENGINE_OUTDIR}/Plugin.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Plugin.o ${ENGINE_SRCDIR}/plugin/Plugin.cpp
${ENGINE_OUTDIR}/PluginKernel.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PluginKernel.o ${ENGINE_SRCDIR}/plugin/PluginKernel.cpp
${ENGINE_OUTDIR}/PluginMessage.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PluginMessage.o ${ENGINE_SRCDIR}/plugin/PluginMessage.cpp
${ENGINE_OUTDIR}/PluginServer.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PluginServer.o ${ENGINE_SRCDIR}/plugin/PluginServer.cpp

# engine_recordbreaker
${ENGINE_OUTDIR}/RecordBreaker.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RecordBreaker.o ${ENGINE_SRCDIR}/recordbreaker/RecordBreaker.cpp
${ENGINE_OUTDIR}/RecordBreakerConfig.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RecordBreakerConfig.o ${ENGINE_SRCDIR}/recordbreaker/RecordBreakerConfig.cpp
${ENGINE_OUTDIR}/RecordBreakerConfigParam.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RecordBreakerConfigParam.o ${ENGINE_SRCDIR}/recordbreaker/RecordBreakerConfigParam.cpp

# engine_searcher
${ENGINE_OUTDIR}/QueryResolver.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/QueryResolver.o ${ENGINE_SRCDIR}/searcher/QueryResolver.cpp
${ENGINE_OUTDIR}/Searcher.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Searcher.o ${ENGINE_SRCDIR}/searcher/Searcher.cpp

# engine_token
${ENGINE_OUTDIR}/Token.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Token.o ${ENGINE_SRCDIR}/tokenizer/Token.cpp

# engine_unittest
${ENGINE_OUTDIR}/AdornerDecoratorTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerDecoratorTest.o ${ENGINE_SRCDIR}/unittest/AdornerDecoratorTest.cpp
${ENGINE_OUTDIR}/AdornerHandlerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerHandlerTest.o ${ENGINE_SRCDIR}/unittest/AdornerHandlerTest.cpp
${ENGINE_OUTDIR}/AdornerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/AdornerTest.o ${ENGINE_SRCDIR}/unittest/AdornerTest.cpp
${ENGINE_OUTDIR}/BasicTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/BasicTest.o ${ENGINE_SRCDIR}/unittest/BasicTest.cpp
${ENGINE_OUTDIR}/CollectionTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/CollectionTest.o ${ENGINE_SRCDIR}/unittest/CollectionTest.cpp
${ENGINE_OUTDIR}/ColManagerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ColManagerTest.o ${ENGINE_SRCDIR}/unittest/ColManagerTest.cpp
${ENGINE_OUTDIR}/ColMetaTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ColMetaTest.o ${ENGINE_SRCDIR}/unittest/ColMetaTest.cpp
${ENGINE_OUTDIR}/DbBlobTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbBlobTest.o ${ENGINE_SRCDIR}/unittest/DbBlobTest.cpp
${ENGINE_OUTDIR}/DbColMetaTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbColMetaTest.o ${ENGINE_SRCDIR}/unittest/DbColMetaTest.cpp
${ENGINE_OUTDIR}/DbDocMetaTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbDocMetaTest.o ${ENGINE_SRCDIR}/unittest/DbDocMetaTest.cpp
${ENGINE_OUTDIR}/DbDocTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbDocTest.o ${ENGINE_SRCDIR}/unittest/DbDocTest.cpp
${ENGINE_OUTDIR}/DbFieldMetaTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbFieldMetaTest.o ${ENGINE_SRCDIR}/unittest/DbFieldMetaTest.cpp
${ENGINE_OUTDIR}/DbManagerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbManagerTest.o ${ENGINE_SRCDIR}/unittest/DbManagerTest.cpp
${ENGINE_OUTDIR}/DbMapTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbMapTest.o ${ENGINE_SRCDIR}/unittest/DbMapTest.cpp
${ENGINE_OUTDIR}/DbNdxTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbNdxTest.o ${ENGINE_SRCDIR}/unittest/DbNdxTest.cpp
${ENGINE_OUTDIR}/DbSearchRecordTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DbSearchRecordTest.o ${ENGINE_SRCDIR}/unittest/DbSearchRecordTest.cpp
${ENGINE_OUTDIR}/DocMetaTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DocMetaTest.o ${ENGINE_SRCDIR}/unittest/DocMetaTest.cpp
${ENGINE_OUTDIR}/DocRecordTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DocRecordTest.o ${ENGINE_SRCDIR}/unittest/DocRecordTest.cpp
${ENGINE_OUTDIR}/EntryTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/EntryTest.o ${ENGINE_SRCDIR}/unittest/EntryTest.cpp
${ENGINE_OUTDIR}/EnumStringTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/EnumStringTest.o ${ENGINE_SRCDIR}/unittest/EnumStringTest.cpp
${ENGINE_OUTDIR}/FileSystemTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FileSystemTest.o ${ENGINE_SRCDIR}/unittest/FileSystemTest.cpp
${ENGINE_OUTDIR}/IndexerHandlerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerHandlerTest.o ${ENGINE_SRCDIR}/unittest/IndexerHandlerTest.cpp
${ENGINE_OUTDIR}/IndexerItemTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerItemTest.o ${ENGINE_SRCDIR}/unittest/IndexerItemTest.cpp
${ENGINE_OUTDIR}/IndexerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/IndexerTest.o ${ENGINE_SRCDIR}/unittest/IndexerTest.cpp
${ENGINE_OUTDIR}/Md5Test.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Md5Test.o ${ENGINE_SRCDIR}/unittest/Md5Test.cpp
${ENGINE_OUTDIR}/ParserTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ParserTest.o ${ENGINE_SRCDIR}/unittest/ParserTest.cpp
${ENGINE_OUTDIR}/PluginXmlExecTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PluginXmlExecTest.o ${ENGINE_SRCDIR}/unittest/PluginXmlExecTest.cpp
${ENGINE_OUTDIR}/PluginXsltTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/PluginXsltTest.o ${ENGINE_SRCDIR}/unittest/PluginXsltTest.cpp
${ENGINE_OUTDIR}/QueryParserTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/QueryParserTest.o ${ENGINE_SRCDIR}/unittest/QueryParserTest.cpp
${ENGINE_OUTDIR}/RecordBreakerTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/RecordBreakerTest.o ${ENGINE_SRCDIR}/unittest/RecordBreakerTest.cpp
${ENGINE_OUTDIR}/SearcherTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/SearcherTest.o ${ENGINE_SRCDIR}/unittest/SearcherTest.cpp
${ENGINE_OUTDIR}/StdStringTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/StdStringTest.o ${ENGINE_SRCDIR}/unittest/StdStringTest.cpp
${ENGINE_OUTDIR}/UnicodeUtilTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/UnicodeUtilTest.o ${ENGINE_SRCDIR}/unittest/UnicodeUtilTest.cpp
${ENGINE_OUTDIR}/UnitTestSuites.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/UnitTestSuites.o ${ENGINE_SRCDIR}/unittest/UnitTestSuites.cpp
${ENGINE_OUTDIR}/XercesTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/XercesTest.o ${ENGINE_SRCDIR}/unittest/XercesTest.cpp
${ENGINE_OUTDIR}/ZlibTest.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ZlibTest.o ${ENGINE_SRCDIR}/unittest/ZlibTest.cpp

# engine_util
${ENGINE_OUTDIR}/ColMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/ColMeta.o ${ENGINE_SRCDIR}/util/ColMeta.cpp
${ENGINE_OUTDIR}/DocMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DocMeta.o ${ENGINE_SRCDIR}/util/DocMeta.cpp
${ENGINE_OUTDIR}/DocRecord.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/DocRecord.o ${ENGINE_SRCDIR}/util/DocRecord.cpp
${ENGINE_OUTDIR}/Entry.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/Entry.o ${ENGINE_SRCDIR}/util/Entry.cpp
${ENGINE_OUTDIR}/FieldMeta.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/FieldMeta.o ${ENGINE_SRCDIR}/util/FieldMeta.cpp
${ENGINE_OUTDIR}/SearchCrit.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/SearchCrit.o ${ENGINE_SRCDIR}/util/SearchCrit.cpp
${ENGINE_OUTDIR}/SearchRecord.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/SearchRecord.o ${ENGINE_SRCDIR}/util/SearchRecord.cpp
${ENGINE_OUTDIR}/WordEntries.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/WordEntries.o ${ENGINE_SRCDIR}/util/WordEntries.cpp
${ENGINE_OUTDIR}/WordEntry.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/WordEntry.o ${ENGINE_SRCDIR}/util/WordEntry.cpp
${ENGINE_OUTDIR}/WordInt.o:
	${CC} ${ENGINE_INCLUDES} ${CMP} -o ${ENGINE_OUTDIR}/WordInt.o ${ENGINE_SRCDIR}/util/WordInt.cpp

# *** plugin_xmlexec ***

# plugin_xmlexec
${PLUGIN_XMLEXEC_OUTDIR}/PluginXmlExec.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/PluginXmlExec.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/PluginXmlExec.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlExecCmd.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlExecCmd.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/XmlExecCmd.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlExecHandler.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlExecHandler.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/XmlExecHandler.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlExecParser.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlExecParser.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/XmlExecParser.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlExecRunner.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlExecRunner.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/XmlExecRunner.cpp

# plugin_xmlexec_core
${PLUGIN_XMLEXEC_OUTDIR}/XmlReq.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReq.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReq.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqCol.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqCol.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqCol.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDb.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDb.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqDb.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDoc.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqDoc.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqDoc.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqIndex.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqIndex.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqIndex.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInfo.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInfo.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqInfo.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInit.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqInit.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqInit.cpp
${PLUGIN_XMLEXEC_OUTDIR}/XmlReqSearch.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XMLEXEC_OUTDIR}/XmlReqSearch.o ${PLUGIN_XMLEXEC_SRCDIR}/plugin/xmlexec/core/XmlReqSearch.cpp

# *** plugin_xslt ***

# plugin_xslt
${PLUGIN_XSLT_OUTDIR}/PluginXslt.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XSLT_OUTDIR}/PluginXslt.o ${PLUGIN_XSLT_SRCDIR}/plugin/xslt/PluginXslt.cpp
${PLUGIN_XSLT_OUTDIR}/XsltManager.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XSLT_OUTDIR}/XsltManager.o ${PLUGIN_XSLT_SRCDIR}/plugin/xslt/XsltManager.cpp
${PLUGIN_XSLT_OUTDIR}/XsltTransformer.o:
	${CC} ${INCLUDES} ${CMP} -o ${PLUGIN_XSLT_OUTDIR}/XsltTransformer.o ${PLUGIN_XSLT_SRCDIR}/plugin/xslt/XsltTransformer.cpp
