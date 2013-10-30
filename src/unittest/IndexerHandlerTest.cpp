/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerHandlerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerHandlerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbNdx.hpp"
#include "../parser/IndexerHandler.hpp"
#include "../parser/Parser.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(IndexerHandlerTest, Atoll::UnitTestSuites::Indexer());

IndexerHandlerTest::IndexerHandlerTest() :
	mDbMgr(0),
	mParser(0),
	mIndexerHandler(0)
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


IndexerHandlerTest::~IndexerHandlerTest()
{
}
//------------------------------------------------------------------------------

void IndexerHandlerTest::setUp(void)
{
	mEngineEnv = new EngineEnv;
	if (!mEngineEnv->CreateDbEnv(Atoll::UnitTestSuites::GetDbHome())) {
		delete mEngineEnv;
		CPPUNIT_ASSERT_MESSAGE("Create Db env error", false);
	}
	mEngineEnv->SetColId(Atoll::UnitTestSuites::GetColId());

	// Create the database manager
	mDbMgr = new DbManager(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());
	mDbMgr->SetClearOnOpen(true);
	mDbMgr->SetRemoveOnClose(true);

	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	mParser = new Parser(SAX2XMLReader::Val_Never, catalogFile, eTypHandlerIndexer);

	// Initialize the indexer
	mIndexerHandler = static_cast<IndexerHandler *>(mParser->GetHandler());
	mIndexerHandler->SetDbManager(mDbMgr);
	std::string indexerConfig = mPath + "indexer_config_test.xml";
	mIndexerHandler->LoadIndexerConfig(indexerConfig);
}
//------------------------------------------------------------------------------

void IndexerHandlerTest::tearDown(void)
{
	delete mParser;
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void IndexerHandlerTest::testIndexerHandler(void)
{
	bool isOk;
	UnicodeString docName; // Empty => no database document storage
	std::string fullFileName;
	unsigned int numDoc;

	// Parse file utf8_test.xml
	numDoc = 104;
	mIndexerHandler->SetDocRef(numDoc);
	fullFileName = mPath + "utf8_test.xml";
	isOk = mParser->XercesParse(fullFileName);
  CPPUNIT_ASSERT(isOk);
	CheckDatabaseContent("emph", 1, "5e", "EntrySet: (count = 1) [ 104-0-2 ]"); // "em ~ 1 1"
	CheckDatabaseContent("bold", 4, "spices", "EntrySet: (count = 1) [ 104-0-9 ]"); // "b ~ 1 1"
	CheckDatabaseContent("bold", 4, "best", "EntrySet: (count = 1) [ 104-0-100002 ]"); // "b ~ 1 1"
	CheckDatabaseContent("bold", 4, "care", "EntrySet: (count = 1) [ 104-0-100004 ]"); // "b ~ 1 1"
	CheckDatabaseContent("bold", 4, "two", "EntrySet: (count = 1) [ 104-0-19 ]"); // "b ~ 1 1"
	CheckDatabaseContent("item", 6, "all", "EntrySet: (count = 2) [ 104-0-7 104-0-8 ]"); // "item ~ 1 1"

	// Parse file test_xhtml.xml
	numDoc = 1;
	mIndexerHandler->SetDocRef(numDoc);
	fullFileName = mPath + "test_xhtml.xhtml";
	isOk = mParser->XercesParse(fullFileName);
  CPPUNIT_ASSERT(isOk);
	CheckDatabaseContent("emph", 2, "5e", "EntrySet: (count = 2) [ 1-0-5 104-0-2 ]"); // "em ~ 1 1"
	CheckDatabaseContent("emph", 2, "first1", "EntrySet: (count = 1) [ 1-0-3 ]"); // "em ~ 1 1"

	// Parse file test_namespace_multi.xml
	numDoc = 1625;
	mIndexerHandler->SetDocRef(numDoc);
	fullFileName = mPath + "test_namespace_multi.xml";
	isOk = mParser->XercesParse(fullFileName);
  CPPUNIT_ASSERT(isOk);
	CheckDatabaseContent("title", 4, "cheaper", "EntrySet: (count = 1) [ 1625-0-1 ]"); // "bk:title ~ 1 1"
}
//------------------------------------------------------------------------------

void IndexerHandlerTest::CheckDatabaseContent(const char *inDbName,
	unsigned long inDbNdxCount, const char *inWord, const char *inStrEntrySet)
{
	// Check the content of the database
	DbNdx *dbNdx;
	unsigned long dbNdxCount;
	std::string dbName(inDbName);
	EntrySet entrySet;
	UnicodeString word;
	std::string strEntrySet;

	dbNdx = mDbMgr->GetDbNdx(dbName);
	dbNdxCount = dbNdx->Count();
  CPPUNIT_ASSERT(dbNdxCount == inDbNdxCount);
	word = inWord;
	dbNdx->GetWordEntries(word, entrySet);
	strEntrySet = EntrySet2String(entrySet);
  CPPUNIT_ASSERT_MESSAGE(inDbName, strEntrySet == inStrEntrySet);
}
//------------------------------------------------------------------------------
