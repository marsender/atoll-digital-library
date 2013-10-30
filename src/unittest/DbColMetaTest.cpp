/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbColMetaTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbColMetaTest.hpp"
#include "UnitTestSuites.hpp"
#include "../database/DbColMeta.hpp"
#include "../database/DbManager.hpp"
#include "../engine/EngineEnv.hpp"
#include "../util/ColMeta.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbColMetaTest, Atoll::UnitTestSuites::Database());

DbColMetaTest::DbColMetaTest()
{
}
//------------------------------------------------------------------------------

DbColMetaTest::~DbColMetaTest()
{
}
//------------------------------------------------------------------------------

void DbColMetaTest::setUp(void)
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
	UnicodeString dbName("DbColMetaTest");
	mDbColMeta = mDbMgr->GetDbColMeta(dbName);

	// Clear the database
	mDbColMeta->clear();
}
//------------------------------------------------------------------------------

void DbColMetaTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbColMetaTest::testAddColMeta(void)
{
	bool isOk;
	unsigned long numCol1 = 1, numCol2 = 2;
	// Init record keys
	UnicodeString colId1 = "ColId 1";
	UnicodeString colId2 = "ColId 2";
	// Init titles
	UnicodeString title1 = "Title 1";
	UnicodeString title2 = "Title 2";
	// Init description (plain text)
	UnicodeString desc1("Text content 1");
	UnicodeString desc2("Text content 2");
	// Init salt (plain text)
	UnicodeString salt1("Salt 1");
	UnicodeString salt2("Salt 2");

	ColMeta cr;
	cr.mColNum = numCol1;
	cr.mColId = colId1;
	cr.mColTitle = title1;
	cr.mColDesc = desc1;
	cr.mColSalt = salt1;

	// Add one record
	isOk = mDbColMeta->AddColMeta(cr);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);
	// Add same record
	isOk = mDbColMeta->AddColMeta(cr);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);

	// Get this record
	cr.Clear();
	cr.mColId = colId1;
	isOk = mDbColMeta->GetColMeta(cr);
	CPPUNIT_ASSERT_MESSAGE("Get record error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record content error", cr.mColNum == numCol1 && cr.mColId == colId1
		&& cr.mColTitle == title1 && cr.mColDesc == desc1 && cr.mColSalt == salt1);

	// Remove this record in the database
	mDbColMeta->DelColMeta(colId1);
	unsigned long count = mDbColMeta->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add one record
	mDbColMeta->AddColMeta(cr);

	// Add another record
	cr.mColNum = numCol2;
	cr.mColId = colId2;
	cr.mColTitle = title2;
	cr.mColDesc = desc2;
	cr.mColSalt = salt2;
	mDbColMeta->AddColMeta(cr);

	// Get the first record
	cr.Clear();
	cr.mColId = colId1;
	isOk = mDbColMeta->GetColMeta(cr);
	CPPUNIT_ASSERT_MESSAGE("Get ColMeta 1 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record ColMeta 1 content error", cr.mColNum == numCol1 && cr.mColId == colId1
		&& cr.mColTitle == title1 && cr.mColDesc == desc1 && cr.mColSalt == salt1);

	// Get the second record
	cr.Clear();
	cr.mColId = colId2;
	isOk = mDbColMeta->GetColMeta(cr);
	CPPUNIT_ASSERT_MESSAGE("Get ColMeta 2 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record ColMeta 2 content error", cr.mColNum == numCol2 && cr.mColId == colId2
		&& cr.mColTitle == title2 && cr.mColDesc == desc2 && cr.mColSalt == salt2);
}
//------------------------------------------------------------------------------

void DbColMetaTest::testColMetaVector(void)
{
	bool isOk;
	ColMeta cr;
	unsigned long numCol1 = 1, numCol2 = 2;
	// Init record keys
	UnicodeString colId1 = "ColId 1";
	UnicodeString colId2 = "ColId 2";
	// Init titles
	UnicodeString title1 = "Title 1";
	UnicodeString title2 = "Title 2";
	// Init description (plain text)
	UnicodeString desc1("Text content 1");
	UnicodeString desc2("Text content 2");
	// Init salt (plain text)
	UnicodeString salt1("Salt 1");
	UnicodeString salt2("Salt 2");

	// Add one record
	cr.mColNum = numCol1;
	cr.mColId = colId1;
	cr.mColTitle = title1;
	cr.mColDesc = desc1;
	cr.mColSalt = salt1;
	mDbColMeta->AddColMeta(cr);

	// Add another record
	cr.mColNum = numCol2;
	cr.mColId = colId2;
	cr.mColTitle = title2;
	cr.mColDesc = desc2;
	cr.mColSalt = salt2;
	mDbColMeta->AddColMeta(cr);

	// Get the vector of all collection metadata records
	ColMetaVector vector;
	isOk = mDbColMeta->GetColMetaVector(vector);
	CPPUNIT_ASSERT_MESSAGE("Get collection metadata vector", isOk && vector.size() == 2);

	// Loop through the vector
	ColMetaVector::const_iterator it = vector.begin();
	ColMetaVector::const_iterator itEnd = vector.end();
	for (; it != itEnd; ++it) {
		const ColMeta &colMeta = (*it);
		if (it == vector.begin()) {
			CPPUNIT_ASSERT_MESSAGE("ColMeta 1 error", colMeta.mColNum == numCol1
				&& colMeta.mColId == colId1 && colMeta.mColTitle == title1
				&& colMeta.mColDesc == desc1 && colMeta.mColSalt == salt1);
		}
		else {
			CPPUNIT_ASSERT_MESSAGE("ColMeta 2 error", colMeta.mColNum == numCol2
				&& colMeta.mColId == colId2 && colMeta.mColTitle == title2
				&& colMeta.mColDesc == desc2 && colMeta.mColSalt == salt2);
		}
	}
}
//------------------------------------------------------------------------------
