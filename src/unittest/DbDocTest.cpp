/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbDocTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbDocTest.hpp"
#include "UnitTestSuites.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbDoc.hpp"
#include "../engine/EngineEnv.hpp"
#include "../util/DocRecord.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbDocTest, Atoll::UnitTestSuites::Database());

DbDocTest::DbDocTest()
{
}
//------------------------------------------------------------------------------


DbDocTest::~DbDocTest()
{
}
//------------------------------------------------------------------------------

void DbDocTest::setUp(void)
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
	unsigned int docNum = 1640;
	mDbDoc = mDbMgr->GetDbDoc(docNum);

	// Clear the database
	mDbDoc->clear();
}
//------------------------------------------------------------------------------

void DbDocTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbDocTest::testAddDocRecord(void)
{
	unsigned long pge1 = 0, pge2 = 6;
	unsigned long pos1 = DEF_LongMax, pos2 = 12;
	// Init record keys
	UnicodeString key1 = "page1";
	UnicodeString key2 = "page2";
	// Init record content (plain text)
	UnicodeString texte1("Text content");
	UnicodeString texte2("<title>Doc title</title>text content");

	DocRecord dr;
	dr.mPge = pge1;
	dr.mPos = pos1;
	dr.mTexte = texte1;

	// Add one key and record
	mDbDoc->AddDocRecord(key1, dr);
	// Add same key and record
	mDbDoc->AddDocRecord(key1, dr);

	// Get this key record
	dr.Clear();
	mDbDoc->GetDocRecord(key1, dr);
	CPPUNIT_ASSERT_MESSAGE("Get record error", dr.mPge == pge1 && dr.mPos == pos1 && dr.mTexte == texte1);

	// Remove this key record in the database
	mDbDoc->DelDocRecord(key1);
	unsigned long count = mDbDoc->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add one key and record
	mDbDoc->AddDocRecord(key1, dr);

	// Add another key and record
	dr.mPge = pge2;
	dr.mPos = pos2;
	dr.mTexte = texte2;
	mDbDoc->AddDocRecord(key2, dr);

	// Get the first key record
	dr.Clear();
	mDbDoc->GetDocRecord(key1, dr);
	CPPUNIT_ASSERT_MESSAGE("Get record error", dr.mPge == pge1 && dr.mPos == pos1 && dr.mTexte == texte1);

	// Get the second key record
	dr.Clear();
	mDbDoc->GetDocRecord(key2, dr);
	CPPUNIT_ASSERT_MESSAGE("Get record error", dr.mPge == pge2 && dr.mPos == pos2 && dr.mTexte == texte2);
}
//------------------------------------------------------------------------------

void DbDocTest::testDocRecordVector(void)
{
	bool isOk;
	DocRecord dr;
	unsigned long pge1 = 0, pge2 = 6;
	unsigned long pos1 = DEF_LongMax, pos2 = 12;
	// Init record keys
	UnicodeString key1 = "page1";
	UnicodeString key2 = "page2";
	// Init record content (plain text)
	UnicodeString texte1("Text content");
	UnicodeString texte2("<title>Doc title</title>text content");

	// Add one key and record
	dr.mPge = pge1;
	dr.mPos = pos1;
	dr.mTexte = texte1;
	mDbDoc->AddDocRecord(key1, dr);

	// Add another key and record
	dr.mPge = pge2;
	dr.mPos = pos2;
	dr.mTexte = texte2;
	mDbDoc->AddDocRecord(key2, dr);

	// Get the vector of all document records
	DocRecordVector vector;
	isOk = mDbDoc->GetDocRecordVector(vector);
	CPPUNIT_ASSERT_MESSAGE("Get document vector", isOk && vector.size() == 2);

	// Loop through the vector
	DocRecordVector::const_iterator it = vector.begin();
	DocRecordVector::const_iterator itEnd = vector.end();
	for (; it != itEnd; ++it) {
		const DocRecord &docRecord = (*it);
		if (it == vector.begin()) {
			CPPUNIT_ASSERT_MESSAGE("DocRecord 1 error", docRecord.mPge == pge1
				&& docRecord.mPos == pos1 && docRecord.mTexte == texte1);
		}
		else {
			CPPUNIT_ASSERT_MESSAGE("DocRecord 2 error", docRecord.mPge == pge2
				&& docRecord.mPos == pos2 && docRecord.mTexte == texte2);
		}
	}
}
//------------------------------------------------------------------------------
