/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbBlobTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbBlobTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbBlob.hpp"
#include "../common/BinaryBuffer.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbBlobTest, Atoll::UnitTestSuites::Database());

DbBlobTest::DbBlobTest()
{
}
//------------------------------------------------------------------------------


DbBlobTest::~DbBlobTest()
{
}
//------------------------------------------------------------------------------

void DbBlobTest::setUp(void)
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

	// Open database
	std::string dbName("DbBlobTest");
	mDbBlob = mDbMgr->GetDbBlob(dbName);

	// Clear the database
	mDbBlob->clear();
}
//------------------------------------------------------------------------------

void DbBlobTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbBlobTest::testMain(void)
{
	bool isOk;
	std::string sKey, sTest;
	BinaryBuffer binBuf, binBufTest;

	// Init the key
	sKey = "79be2f3810fcd6257d0dd3491146ea3b";

	// Init a binary buffer
	sTest = "md5sum1_1 Word to search 1_1..md5sum1_2 Word to search 1_2";
	binBuf.write(sTest.c_str(), sTest.size());

	// Add the key => value in the database
	isOk = mDbBlob->AddElement(sKey, binBuf);
	CPPUNIT_ASSERT_MESSAGE("Add element error", isOk);

	// Retrieve the key => value from the database
	isOk = mDbBlob->GetElement(sKey, binBufTest);
  CPPUNIT_ASSERT_MESSAGE("Get element error", isOk);
  CPPUNIT_ASSERT_MESSAGE("Element content error", binBuf == binBufTest);

	// Remove the key => value from the database
	isOk = mDbBlob->DelElement(sKey);
  CPPUNIT_ASSERT_MESSAGE("Del element error", isOk);
  CPPUNIT_ASSERT_MESSAGE("Db size error", mDbBlob->Count() == 0);
}
//------------------------------------------------------------------------------
