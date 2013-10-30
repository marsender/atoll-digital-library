/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbMapTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbMapTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbMap.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbMapTest, Atoll::UnitTestSuites::Database());

DbMapTest::DbMapTest()
{
}
//------------------------------------------------------------------------------


DbMapTest::~DbMapTest()
{
}
//------------------------------------------------------------------------------

void DbMapTest::setUp(void)
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
	std::string dbName("DbMapTest");
	mDbMap = mDbMgr->GetDbMap(dbName);

	// Clear the database
	mDbMap->clear();
}
//------------------------------------------------------------------------------

void DbMapTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbMapTest::testUnicodeStringToIntMap(void)
{
	bool isOk;
	size_t mapSize;
	unsigned long dbMapSize;
	UnicodeString key1 = "key1";
	UnicodeString key2 = "key2";
	UnicodeString key3 = "key3";
	unsigned int data1, data2, data3;
	Common::UnicodeStringToIntMap map, mapTest;

	CPPUNIT_ASSERT(DEF_LongMax == 0xffffffff); // 4 bytes

	// Save map
	data1 = 1;
	data2 = 2;
	data3 = DEF_LongMax;
	map[key1] = data1;
	map[key2] = data2;
	map[key3] = data3;
	mDbMap->SaveMapToDb(map);
	dbMapSize = mDbMap->Count();
	CPPUNIT_ASSERT_MESSAGE("Db count error", dbMapSize == map.size());

	// Load map
	isOk = mDbMap->LoadMapFromDb(mapTest);
  CPPUNIT_ASSERT_MESSAGE("Map load error", isOk);
	mapSize = mapTest.size();
  CPPUNIT_ASSERT_MESSAGE("Map count error", mapSize == mDbMap->Count());
  CPPUNIT_ASSERT_MESSAGE("Map content error", map == mapTest);
}
//------------------------------------------------------------------------------
