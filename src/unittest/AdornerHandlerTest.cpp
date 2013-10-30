/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerHandlerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "AdornerHandlerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbDoc.hpp"
#include "../engine/EngineEnv.hpp"
#include "../parser/AdornerHandler.hpp"
#include "../parser/Parser.hpp"
#include "../util/DocRecord.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AdornerHandlerTest, Atoll::UnitTestSuites::Adorner());

AdornerHandlerTest::AdornerHandlerTest() :
	mParser(0),
	mAdornerHandler(0)
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


AdornerHandlerTest::~AdornerHandlerTest()
{
}
//------------------------------------------------------------------------------

void AdornerHandlerTest::setUp(void)
{
	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	mParser = new Parser(SAX2XMLReader::Val_Never, catalogFile, eTypHandlerAdorner);

	// Initialize the Adorner
	mAdornerHandler = static_cast<AdornerHandler *>(mParser->GetHandler());
}
//------------------------------------------------------------------------------

void AdornerHandlerTest::tearDown(void)
{
	mAdornerHandler = NULL;

	delete mParser;
	mParser = NULL;
}
//------------------------------------------------------------------------------

void AdornerHandlerTest::testAdornerHandler(void)
{
	bool isOk, wantOutputFile = true;
	unsigned int numDoc = 1;
	std::string testFileName = mPath + "test_adorner_handler.xml";
	std::string outFileName = mPath + "test_adorner_handler_temp.xml";

	mAdornerHandler->SetDocRef(numDoc);
	if (wantOutputFile)
		mAdornerHandler->SetDstFileName(outFileName);

	// utf8_test.xml test_xhtml.xml test_namespace_multi.xml
	std::string fullFileName = mPath + "utf8_test.xml";
	isOk = mParser->XercesParse(fullFileName);
	CPPUNIT_ASSERT(isOk);

	if (wantOutputFile) {
		// Text compare source and destination files
		isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);

		// Delete temporary file
		::remove(outFileName.c_str());
	}
}
//------------------------------------------------------------------------------

void AdornerHandlerTest::testAdornerHandlerOutputString(void)
{
	bool isOk;
	unsigned int numDoc = 1;
	std::string testFileName = mPath + "test_adorner_handler.xml";
	std::string outFileName = mPath + "test_adorner_handler_temp.xml";

	mAdornerHandler->SetDocRef(numDoc);
	mAdornerHandler->EnableOutputString(true);
	UnicodeString &uStr = mAdornerHandler->GetOutputString();

	// utf8_test.xml test_xhtml.xml test_namespace_multi.xml
	std::string fullFileName = mPath + "utf8_test.xml";
	isOk = mParser->XercesParse(fullFileName);
	CPPUNIT_ASSERT(isOk);

	// Save the unicode string into the file
	isOk = UnicodeString2File(uStr, outFileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);

	// Clear output string
	uStr.remove();

	// Text compare source and destination files
	isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
	CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);

	// Delete temporary file
	::remove(outFileName.c_str());
}
//------------------------------------------------------------------------------

void AdornerHandlerTest::testAdornerHandlerRecordBreaker(void)
{
	bool isOk;
	unsigned int numDoc = 1;

	mEngineEnv = new EngineEnv;
	if (!mEngineEnv->CreateDbEnv(Atoll::UnitTestSuites::GetDbHome())) {
		delete mEngineEnv;
		CPPUNIT_ASSERT_MESSAGE("Create Db env error", false);
	}
	mEngineEnv->SetColId(Atoll::UnitTestSuites::GetColId());

	// Create the database manager
	mDbMgr = new DbManager(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());
	mDbMgr->SetClearOnOpen(false); // Don't clear, or nothing will be saved
	mDbMgr->SetRemoveOnClose(false); // Don't remove, or nothing will be saved

	// Init adorner handler
	mAdornerHandler->SetDocRef(numDoc);
	mAdornerHandler->SetDbManager(mDbMgr);

	// Load record breaker config
	std::string configFileName = mPath + "recordbreaker_config_test.xml";
	mAdornerHandler->LoadRecordBreakerConfig(configFileName);
	mAdornerHandler->EnableOutputRecordBreaker(true);

	// Parse
	std::string fullFileName = mPath + "record_breaker_test.xml";
	isOk = mParser->XercesParse(fullFileName);
	CPPUNIT_ASSERT(isOk);

	// Get the result doc
	DbDoc *dbDoc = mDbMgr->GetDbDoc(numDoc);

	// Get the vector of all document records
	DocRecordVector vector;
	isOk = dbDoc->GetDocRecordVector(vector);
	CPPUNIT_ASSERT_MESSAGE("Get document vector", isOk && vector.size() > 1);

	// Loop through the vector
	DocRecordVector::const_iterator it = vector.begin();
	DocRecordVector::const_iterator itEnd = vector.end();
	std::string testFileName, outFileName;
	for (; it != itEnd; ++it) {
		const DocRecord &docRecord = (*it);
		char name[255];
		sprintf(name, "test_record_breaker_%02lu.xml", docRecord.mPge);
		testFileName = mPath + name;
		sprintf(name, "test_record_breaker_%02lu_temp.xml", docRecord.mPge);
		outFileName = mPath + name;
		// Save the unicode string into the file
		isOk = UnicodeString2File(docRecord.mTexte, outFileName.c_str());
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		// Text compare source and destination files
		isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		// Delete temporary file
		::remove(outFileName.c_str());
	}

	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------
