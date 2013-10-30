/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbSearchRecordTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbSearchRecordTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../database/DbSearchRecord.hpp"
#include "../database/DbManager.hpp"
#include "../engine/EngineEnv.hpp"
#include "../util/SearchRecord.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbSearchRecordTest, Atoll::UnitTestSuites::Database());

DbSearchRecordTest::DbSearchRecordTest()
{
}
//------------------------------------------------------------------------------

DbSearchRecordTest::~DbSearchRecordTest()
{
}
//------------------------------------------------------------------------------

void DbSearchRecordTest::setUp(void)
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
	UnicodeString dbName("DbSearchRecordTest");
	mDbSearchRecord = mDbMgr->GetDbSearchRecord(dbName);

	// Clear the database
	mDbSearchRecord->clear();
}
//------------------------------------------------------------------------------

void DbSearchRecordTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbSearchRecordTest::testAddSearchRecord(void)
{
	bool isOk;
	eTypSearchRecord typSearchRecord1 = eTypSearchRecordNormal;
	eTypSearchRecord typSearchRecord2 = eTypSearchRecordCritOnly;
	std::string searchId1, searchId2;
	// Init validity indices
	unsigned int validityIndice1 = 0;
	unsigned int validityIndice2 = 1;
	// Init search criteria vectors
	SearchCrit sc;
	SearchCritVector searchCritVector1, searchCritVector2;
	unsigned long searchCritVectorSize1, searchCritVectorSize2;
	sc.SetIdCrit("md5sum1_1");
	sc.mWrd = "Word to search 1_1";
	searchCritVector1.push_back(sc);
	sc.SetIdCrit("md5sum1_2");
	sc.mWrd = "Word to search 1_2";
	searchCritVector1.push_back(sc);
	sc.SetIdCrit("md5sum2_1");
	sc.mWrd = "Word to search 2_1";
	searchCritVector2.push_back(sc);
	searchCritVectorSize1 = searchCritVector1.size();
	searchCritVectorSize2 = searchCritVector2.size();
	// Init entry sets
	Entry e;
	EntrySet entrySet1, entrySet2;
	unsigned long entrySetSize1, entrySetSize2;
	e.mUiEntDoc = 1;
	e.mUlEntPge = 1;
	e.mUlEntPos = 1;
	entrySet1.insert(e);
	entrySetSize1 = entrySet1.size();
	entrySetSize2 = entrySet2.size();

	// Init search record
	SearchRecord sr;
	sr.mTypSearchRecord = typSearchRecord1;
	sr.mValidityIndice = validityIndice1;
	sr.mSearchCritVector = searchCritVector1;
	sr.mEntrySet = entrySet1;
	//sr.SetSearchId(); // Init the search id with md5 sum
	//searchId1 = sr.mSearchId;

	// Test search record serialisation / deserialisation
	SearchRecord srTest;
	BinaryBuffer buffer;
	sr.ToBinaryBuffer(buffer);
	searchId1 = sr.mSearchId;
	srTest.FromBinaryBuffer(buffer);
	CPPUNIT_ASSERT_MESSAGE("Search record serialisation / deserialisation error",
		srTest.mTypSearchRecord == typSearchRecord1
		&& srTest.mSearchId == searchId1
		&& srTest.mValidityIndice == validityIndice1
		&& srTest.mSearchCritVector.size() == searchCritVectorSize1
		&& srTest.mEntrySet.size() == entrySetSize1);

	// Add one record
	isOk = mDbSearchRecord->AddSearchRecord(sr);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);
	// Add same record
	isOk = mDbSearchRecord->AddSearchRecord(sr);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);
	unsigned long count = mDbSearchRecord->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 1);

	// Get this record
	sr.Clear();
	sr.mTypSearchRecord = typSearchRecord1;
	sr.mSearchId = searchId1;
	isOk = mDbSearchRecord->GetSearchRecord(sr);
	CPPUNIT_ASSERT_MESSAGE("Get record error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record content error", sr.mTypSearchRecord == typSearchRecord1
		&& sr.mSearchId == searchId1
		&& sr.mValidityIndice == validityIndice1
		&& sr.mSearchCritVector.size() == searchCritVectorSize1
		&& sr.mEntrySet.size() == entrySetSize1);

	// Remove this record in the database
	mDbSearchRecord->DelSearchRecord(sr);
	count = mDbSearchRecord->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add one record
	mDbSearchRecord->AddSearchRecord(sr);

	// Add another record
	sr.mTypSearchRecord = typSearchRecord2;
	sr.mValidityIndice = validityIndice2;
	sr.mSearchCritVector = searchCritVector2;
	sr.mEntrySet = entrySet2;
	mDbSearchRecord->AddSearchRecord(sr);
	searchId2 = sr.mSearchId; // Get the search id with md5 sum

	// Get the first record
	sr.Clear();
	sr.mTypSearchRecord = typSearchRecord1;
	sr.mSearchId = searchId1;
	isOk = mDbSearchRecord->GetSearchRecord(sr);
	CPPUNIT_ASSERT_MESSAGE("Get SearchRecord 1 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record SearchRecord 1 content error", sr.mTypSearchRecord == typSearchRecord1
		&& sr.mSearchId == searchId1
		&& sr.mValidityIndice == validityIndice1
		&& sr.mSearchCritVector.size() == searchCritVectorSize1
		&& sr.mEntrySet.size() == entrySetSize1);

	// Get the second record
	sr.Clear();
	sr.mTypSearchRecord = typSearchRecord2;
	sr.mSearchId = searchId2;
	isOk = mDbSearchRecord->GetSearchRecord(sr);
	CPPUNIT_ASSERT_MESSAGE("Get SearchRecord 2 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record SearchRecord 2 content error", sr.mTypSearchRecord == typSearchRecord2
		&& sr.mSearchId == searchId2
		&& sr.mValidityIndice == validityIndice2
		&& sr.mSearchCritVector.size() == searchCritVectorSize2
		&& sr.mEntrySet.size() == entrySetSize2);
}
//------------------------------------------------------------------------------

void DbSearchRecordTest::testSearchRecordVector(void)
{
	bool isOk;
	SearchRecord sr;
	eTypSearchRecord typSearchRecord1 = eTypSearchRecordNormal;
	eTypSearchRecord typSearchRecord2 = eTypSearchRecordCritOnly;
	// Init record keys
	std::string searchId1, searchId2;
	// Init validity indices
	unsigned int validityIndice1 = 0;
	unsigned int validityIndice2 = 1;
	// Init search criteria vectors
	SearchCrit sc;
	SearchCritVector searchCritVector1, searchCritVector2;
	unsigned long searchCritVectorSize1, searchCritVectorSize2;
	sc.SetIdCrit("md5sum1_1");
	sc.mWrd = "Word to search 1_1";
	searchCritVector1.push_back(sc);
	sc.SetIdCrit("md5sum1_2");
	sc.mWrd = "Word to search 1_2";
	searchCritVector1.push_back(sc);
	sc.SetIdCrit("md5sum2_1");
	sc.mWrd = "Word to search 2_1";
	searchCritVector2.push_back(sc);
	searchCritVectorSize1 = searchCritVector1.size();
	searchCritVectorSize2 = searchCritVector2.size();
	// Init entry sets
	Entry e;
	EntrySet entrySet1, entrySet2;
	unsigned long entrySetSize1, entrySetSize2;
	e.mUiEntDoc = 1;
	e.mUlEntPge = 1;
	e.mUlEntPos = 1;
	entrySet2.insert(e);
	entrySetSize1 = entrySet1.size();
	entrySetSize2 = entrySet2.size();

	// Add one record
	sr.mTypSearchRecord = typSearchRecord1;
	sr.mValidityIndice = validityIndice1;
	sr.mSearchCritVector = searchCritVector1;
	sr.mEntrySet = entrySet1;
	mDbSearchRecord->AddSearchRecord(sr);
	searchId1 = sr.mSearchId; // Get the search id with md5 sum

	// Add another record
	sr.mTypSearchRecord = typSearchRecord2;
	sr.mValidityIndice = validityIndice2;
	sr.mSearchCritVector = searchCritVector2;
	sr.mEntrySet = entrySet2;
	mDbSearchRecord->AddSearchRecord(sr);
	searchId2 = sr.mSearchId; // Get the search id with md5 sum

	// Get the vector of all searchRecord from a type
	SearchRecordVector vector;
	isOk = mDbSearchRecord->GetSearchRecordVector(vector, typSearchRecord1);
	CPPUNIT_ASSERT_MESSAGE("Get searchRecord vector", isOk && vector.size() == 1);

	// Get the vector of all searchRecord from any type
	isOk = mDbSearchRecord->GetSearchRecordVector(vector, eTypSearchRecordNone);
	CPPUNIT_ASSERT_MESSAGE("Get searchRecord vector", isOk && vector.size() == 2);

	// Loop through the vector
	SearchRecordVector::const_iterator it = vector.begin();
	SearchRecordVector::const_iterator itEnd = vector.end();
	for (; it != itEnd; it++) {
		const SearchRecord &searchRecord = (*it);
		if (it == vector.begin()) {
			CPPUNIT_ASSERT_MESSAGE("SearchRecord 1 error", searchRecord.mTypSearchRecord == typSearchRecord1
				&& searchRecord.mSearchId == searchId1
				&& searchRecord.mValidityIndice == validityIndice1
				&& searchRecord.mSearchCritVector.size() == searchCritVectorSize1
				&& searchRecord.mEntrySet.size() == entrySetSize1);
		}
		else {
			CPPUNIT_ASSERT_MESSAGE("SearchRecord 2 error", searchRecord.mTypSearchRecord == typSearchRecord2
				&& searchRecord.mSearchId == searchId2
				&& searchRecord.mValidityIndice == validityIndice2
				&& searchRecord.mSearchCritVector.size() == searchCritVectorSize2
				&& searchRecord.mEntrySet.size() == entrySetSize2);
		}
	}
}
//------------------------------------------------------------------------------
