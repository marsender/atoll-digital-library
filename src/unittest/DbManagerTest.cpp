/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbManagerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbManagerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/StdString.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbColMeta.hpp"
#include "../database/DbDoc.hpp"
#include "../database/DbDocMeta.hpp"
#include "../database/DbMap.hpp"
#include "../database/DbNdx.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbManagerTest, Atoll::UnitTestSuites::Database());

DbManagerTest::DbManagerTest()
{
}
//------------------------------------------------------------------------------


DbManagerTest::~DbManagerTest()
{
}
//------------------------------------------------------------------------------

void DbManagerTest::setUp(void)
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

void DbManagerTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbName(void)
{
	DbColMeta *dbCol;
	DbDoc *dbDoc;
	DbNdx *dbNdx;
	std::string dbName, fullDbName1, fullDbName2;
	UnicodeString uDbName;

	// Test identity of string / UnicodeString full db name
	dbName = "DbNameTesté";
	Common::ConvertString2UnicodeString(uDbName, dbName);
	fullDbName1 = mDbMgr->GetFullDbName(dbName);
	fullDbName2 = mDbMgr->GetFullDbName(uDbName);
	CPPUNIT_ASSERT(fullDbName1 == fullDbName2);

	// Test UnicodeString ColMeta database name
	uDbName = "DbColMetaTesté";
	Common::ConvertUnicodeString2String(dbName, uDbName);
	dbCol = mDbMgr->GetDbColMeta(uDbName);
	CPPUNIT_ASSERT(dbCol->getDbName() == mDbMgr->GetFullDbName(dbName));

	// Test unsigned int document database name
	unsigned int intDbName = 1640;
	dbName.clear();
	Common::StdString::AppendUInt(dbName, intDbName);
	dbDoc = mDbMgr->GetDbDoc(intDbName);
	CPPUNIT_ASSERT(dbDoc->getDbName() == mDbMgr->GetFullDbName(dbName));

	// Test std::string database name
	dbName = "DbNdxTesté";
	dbNdx = mDbMgr->GetDbNdx(dbName);
	CPPUNIT_ASSERT(dbNdx->getDbName() == mDbMgr->GetFullDbName(dbName));
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbColMetaPointer(void)
{
	DbColMeta *dbCol1, *dbCol2;
	UnicodeString dbName("DbColMetaTesté"), dbNameOther("DbColMetaTestOtheré");

	// Check if the database pointer is stable for the same database
	dbCol1 = mDbMgr->GetDbColMeta(dbName);
	dbCol2 = mDbMgr->GetDbColMeta(dbName);
  CPPUNIT_ASSERT(dbCol1 == dbCol2);

	// Check if the database pointer is stable after opening another database
	mDbMgr->GetDbColMeta(dbNameOther);
	dbCol2 = mDbMgr->GetDbColMeta(dbName);
  CPPUNIT_ASSERT(dbCol1 == dbCol2);
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbDocMetaPointer(void)
{
	DbDocMeta *dbDoc1, *dbDoc2;
	UnicodeString dbName("DbDocMetaTesté"), dbNameOther("DbDocMetaTestOtheré");

	// Check if the database pointer is stable for the same database
	dbDoc1 = mDbMgr->GetDbDocMeta(dbName);
	dbDoc2 = mDbMgr->GetDbDocMeta(dbName);
  CPPUNIT_ASSERT(dbDoc1 == dbDoc2);

	// Check if the database pointer is stable after opening another database
	mDbMgr->GetDbDocMeta(dbNameOther);
	dbDoc2 = mDbMgr->GetDbDocMeta(dbName);
  CPPUNIT_ASSERT(dbDoc1 == dbDoc2);
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbDocPointer(void)
{
	DbDoc *dbDoc1, *dbDoc2;
	unsigned int dbName = 1, dbNameOther = 2;

	// Check if the database pointer is stable for the same database
	dbDoc1 = mDbMgr->GetDbDoc(dbName);
	dbDoc2 = mDbMgr->GetDbDoc(dbName);
  CPPUNIT_ASSERT(dbDoc1 == dbDoc2);

	// Check if the database pointer is stable after opening another database
	mDbMgr->GetDbDoc(dbNameOther);
	dbDoc2 = mDbMgr->GetDbDoc(dbName);
  CPPUNIT_ASSERT(dbDoc1 == dbDoc2);
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbNdxPointer(void)
{
	DbNdx *dbNdx1, *dbNdx2;
	std::string dbName("DbNdxTesté"), dbNameOther("DbNdxTestOtheré");

	// Check if the database pointer is stable for the same database
	dbNdx1 = mDbMgr->GetDbNdx(dbName);
	dbNdx2 = mDbMgr->GetDbNdx(dbName);
  CPPUNIT_ASSERT(dbNdx1 == dbNdx2);

	// Check if the database pointer is stable after opening another database
	mDbMgr->GetDbNdx(dbNameOther);
	dbNdx2 = mDbMgr->GetDbNdx(dbName);
  CPPUNIT_ASSERT(dbNdx1 == dbNdx2);
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbMapPointer(void)
{
	DbMap *dbMap1, *dbMap2;
	std::string dbName("DbMapTesté"), dbNameOther("DbMapTestOtheré");

	// Check if the database pointer is stable for the same database
	dbMap1 = mDbMgr->GetDbMap(dbName);
	dbMap2 = mDbMgr->GetDbMap(dbName);
  CPPUNIT_ASSERT(dbMap1 == dbMap2);

	// Check if the database pointer is stable after opening another database
	mDbMgr->GetDbMap(dbNameOther);
	dbMap2 = mDbMgr->GetDbMap(dbName);
  CPPUNIT_ASSERT(dbMap1 == dbMap2);
}
//------------------------------------------------------------------------------

void DbManagerTest::testDbClear(void)
{
	DbNdx *dbNdx;
	std::string dbName("DbNdxTesté");
	UnicodeString word = "test";
	Entry e(1, 1, 1);

	mDbMgr->CloseAllDb();

	// Test clear
	dbNdx = mDbMgr->GetDbNdx(dbName);
	dbNdx->clear();
  CPPUNIT_ASSERT(dbNdx->Count() == 0);
	dbNdx->AddWordEntry(word, e);
  CPPUNIT_ASSERT(dbNdx->Count() == 1);
	dbNdx->clear();
  CPPUNIT_ASSERT(dbNdx->Count() == 0);
	mDbMgr->CloseDbNdx(dbName);

	// Test clear on open
	mDbMgr->SetClearOnOpen(true);
	mDbMgr->SetRemoveOnClose(false);
	dbNdx = mDbMgr->GetDbNdx(dbName);
	dbNdx->AddWordEntry(word, e);
	mDbMgr->CloseDbNdx(dbName);
	dbNdx = mDbMgr->GetDbNdx(dbName);
  CPPUNIT_ASSERT(dbNdx->Count() == 0);
	mDbMgr->CloseDbNdx(dbName);

	// Test remove on close
	mDbMgr->SetClearOnOpen(false);
	mDbMgr->SetRemoveOnClose(true);
	dbNdx = mDbMgr->GetDbNdx(dbName);
	dbNdx->AddWordEntry(word, e);
	mDbMgr->CloseDbNdx(dbName);
	dbNdx = mDbMgr->GetDbNdx(dbName);
  CPPUNIT_ASSERT(dbNdx->Count() == 0);
}
//------------------------------------------------------------------------------
