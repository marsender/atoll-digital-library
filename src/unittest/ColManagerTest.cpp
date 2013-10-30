/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColManagerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ColManagerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../collection/ColManager.hpp"
#include "../engine/EngineEnv.hpp"
#include "../util/ColMeta.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ColManagerTest, Atoll::UnitTestSuites::Collection());

ColManagerTest::ColManagerTest()
{
}
//------------------------------------------------------------------------------


ColManagerTest::~ColManagerTest()
{
}
//------------------------------------------------------------------------------

void ColManagerTest::setUp(void)
{
	mEngineEnv = new EngineEnv;
	if (!mEngineEnv->CreateDbEnv(Atoll::UnitTestSuites::GetDbHome())) {
		delete mEngineEnv;
		CPPUNIT_ASSERT_MESSAGE("Create Db env error", false);
	}

	// Create a collection manager
	mColManager = new ColManager(mEngineEnv->GetDbEnv());

	// Remove all collections
	mColManager->ClearColManager();
}
//------------------------------------------------------------------------------

void ColManagerTest::tearDown(void)
{
	delete mColManager;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void ColManagerTest::testAddRemoveCollection(void)
{
	bool isOk;
	bool isLogError;
	UnicodeString colId1, colId2;
	unsigned int colNum, numColExpect1 = 1, numColExpect2 = 2;
	ColMeta colMeta1, colMeta2;

	// Add a collection
	colMeta1.mColId = "ColTest 1";
	colMeta1.mColTitle = "Collection 1 title";
	colMeta1.mColDesc = "Collection 1 description";
	colMeta1.mColSalt = "Collection 1 salt";
	colMeta1.mColNum = 0; // Unused
	isOk = mColManager->AddCollection(colId1, colMeta1);
	CPPUNIT_ASSERT_MESSAGE("Add collection error", isOk);
	CPPUNIT_ASSERT(colMeta1.mColId == colId1);
	colNum = mColManager->GetCollectionNumber(colMeta1.mColId);
	CPPUNIT_ASSERT(colNum == numColExpect1);

	// Add another collection
	colMeta2.mColId = ""; // Empty cod id will generate it
	colMeta2.mColTitle = "Collection 2 title";
	colMeta2.mColDesc = "Collection 2 description";
	colMeta2.mColSalt = "Collection 2 salt";
	colMeta2.mColNum = 0; // Unused
	isOk = mColManager->AddCollection(colId2, colMeta2);
	colMeta2.mColId = colId2; // Store the generated col id
	CPPUNIT_ASSERT_MESSAGE("Add collection error", isOk);
	colNum = mColManager->GetCollectionNumber(colMeta2.mColId);
	CPPUNIT_ASSERT(colNum == numColExpect2);

	// Add an already existing collection
	/*
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	isOk = mColManager->AddCollection(colMeta1);
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
	CPPUNIT_ASSERT_MESSAGE("Add same collection error", !isOk);
	*/

	// Check the number of collections
	CPPUNIT_ASSERT_MESSAGE("Collection count", mColManager->GetCollectionCount() == 2);

	// Reopen the collection manager
	delete mColManager;
	mColManager = new ColManager(mEngineEnv->GetDbEnv());

	// Check the number of collections
	CPPUNIT_ASSERT_MESSAGE("Collection count", mColManager->GetCollectionCount() == 2);

	// Check if the collections still exist
	CPPUNIT_ASSERT(mColManager->GetCollectionNumber(colMeta1.mColId) == numColExpect1);
	CPPUNIT_ASSERT(mColManager->GetCollectionNumber(colMeta2.mColId) == numColExpect2);

	// Check a non existing collection
	UnicodeString unknownColId("unknown");
	colNum = mColManager->GetCollectionNumber(unknownColId);
	CPPUNIT_ASSERT_MESSAGE("Unknown collection exists", colNum == DEF_IntMax);

	// Remove a collection
	isOk = mColManager->RemoveCollection(colMeta1.mColId);
	CPPUNIT_ASSERT_MESSAGE("Remove collection error", isOk);
	colNum = mColManager->GetCollectionNumber(colMeta1.mColId);
	CPPUNIT_ASSERT_MESSAGE("Removed collection still exist", colNum == DEF_IntMax);

	// Remove a collection twice
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	isOk = mColManager->RemoveCollection(colMeta1.mColId);
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
	CPPUNIT_ASSERT_MESSAGE("Remove collection twice error", !isOk);

	// Remove all collections
	isOk = mColManager->ClearColManager();
	CPPUNIT_ASSERT_MESSAGE("Remove all collections error", isOk);
	colNum = mColManager->GetCollectionNumber(colMeta2.mColId);
	CPPUNIT_ASSERT_MESSAGE("Removed collection still exist", colNum == DEF_IntMax);
}
//------------------------------------------------------------------------------

void ColManagerTest::testOpenCloseCollection(void)
{
	bool isOk;
	bool isLogError;
	ColMeta colMeta;
	UnicodeString colId;
	Collection *collection;

	// Add a collection
	colMeta.mColId = "ColTest 1";
	colMeta.mColTitle = "Collection 1 title";
	colMeta.mColDesc = "Collection 1 description";
	colMeta.mColSalt = "Collection 1 salt";
	colMeta.mColNum = 0; // Unused
	isOk = mColManager->AddCollection(colId, colMeta);
	CPPUNIT_ASSERT_MESSAGE("Add collection error", isOk);
	colMeta.mColId = colId; // Store the id

	// Open the collection
	collection = mColManager->OpenCollection(colMeta.mColId);
	CPPUNIT_ASSERT_MESSAGE("Cannot open existing collection", collection != NULL);

	// Remove the collection
	isOk = mColManager->RemoveCollection(colMeta.mColId);
	CPPUNIT_ASSERT_MESSAGE("Remove collection error", isOk);

	// Try to reopen the collection
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	collection = mColManager->OpenCollection(colMeta.mColId);
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
	CPPUNIT_ASSERT_MESSAGE("Can open a non existing collection", collection == NULL);
}
//------------------------------------------------------------------------------
