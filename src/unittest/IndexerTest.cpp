/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../database/DbManager.hpp"
#include "../engine/EngineEnv.hpp"
#include "../indexer/Indexer.hpp"
#include "../indexer/IndexerConfigParam.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(IndexerTest, Atoll::UnitTestSuites::Indexer());

IndexerTest::IndexerTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


IndexerTest::~IndexerTest()
{
}
//------------------------------------------------------------------------------

void IndexerTest::setUp(void)
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
}
//------------------------------------------------------------------------------

void IndexerTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void IndexerTest::testIndexContent(void)
{
	unsigned long nbError;

	std::string elem;
	std::string attrName;
	UnicodeString attrValue;
	StringToUnicodeStringMap attrMap;
	UnicodeString text;
	unsigned int numDoc = 1;
	UnicodeString docName; // Empty => no database document storage
	std::string indexerConfig = mPath + "indexer_config_test.xml";

	Indexer *indexer = new Indexer();
	indexer->SetDbManager(mDbMgr);
	indexer->LoadIndexerConfig(indexerConfig);
	indexer->SetIndexerDocRef(numDoc);
	indexer->StartDocument();

	// Get a config parameter from it's name
	std::string configParamName("text");
	IndexerConfigParam *indexerConfigParam = indexer->GetIndexerConfigParam(configParamName);
	CPPUNIT_ASSERT(indexerConfigParam != NULL);
	CPPUNIT_ASSERT(indexerConfigParam->mName == configParamName);
	const std::string &indexDbName = indexerConfigParam->GetIndexDbName();
	CPPUNIT_ASSERT(indexDbName == "text"); //"p ~ 1 1");

	// Opens a node
	elem = "indexer_test";
	attrName = "id";
	attrValue = "123";
	attrMap[attrName] = attrValue;
	indexer->StartNode(elem, attrMap);
	// Appends text
	text = "Full text content";
	indexer->AddCharacters(text.getTerminatedBuffer(), text.length());
	// Close the node
	indexer->EndNode();
	indexer->EndDocument(false);
	nbError = indexer->GetNbError();
	delete indexer;

	CPPUNIT_ASSERT(nbError == 0);
	//CPPUNIT_ASSERT_EQUAL(i, 11);
	//CPPUNIT_ASSERT_MESSAGE("Incorrect i value", i == 11);
	//CPPUNIT_ASSERT_THROW(assertion, AppException);
}
//------------------------------------------------------------------------------
