/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbFieldMetaTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbFieldMetaTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbFieldMeta.hpp"
#include "../database/DbManager.hpp"
#include "../util/FieldMeta.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbFieldMetaTest, Atoll::UnitTestSuites::Database());

DbFieldMetaTest::DbFieldMetaTest()
{
}
//------------------------------------------------------------------------------

DbFieldMetaTest::~DbFieldMetaTest()
{
}
//------------------------------------------------------------------------------

void DbFieldMetaTest::setUp(void)
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
	UnicodeString dbName("DbFieldMetaTest");
	mDbFieldMeta = mDbMgr->GetDbFieldMeta(dbName);

	// Clear the database
	mDbFieldMeta->clear();
}
//------------------------------------------------------------------------------

void DbFieldMetaTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbFieldMetaTest::testAddFieldMeta(void)
{
	bool isOk;
	eTypField typField1 = eTypFieldAll, typField2 = eTypFieldSearch;
	unsigned int fieldOrder1 = 5, fieldOrder2 = 9;
	// Init ids
	UnicodeString fieldId1 = "FieldId 1";
	UnicodeString fieldId2 = "FieldId 2";
	// Init titles
	UnicodeString title1 = "Title 1";
	UnicodeString title2 = "Title 2";

	FieldMeta fm;
	fm.mTypField = typField1;
	fm.mFieldOrder = fieldOrder1;
	fm.SetFieldId(fieldId1);
	fm.mFieldTitle = title1;

	// Add one record
	isOk = mDbFieldMeta->AddFieldMeta(fm);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);
	// Add same record
	isOk = mDbFieldMeta->AddFieldMeta(fm);
	CPPUNIT_ASSERT_MESSAGE("Add record error", isOk);

	// Get this record
	fm.Clear();
	fm.mTypField = typField1;
	fm.SetFieldId(fieldId1);
	isOk = mDbFieldMeta->GetFieldMeta(fm);
	CPPUNIT_ASSERT_MESSAGE("Get record error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record content error", fm.mTypField == typField1
		&& fm.mFieldOrder == fieldOrder1 && fm.GetFieldId() == fieldId1
		&& fm.mFieldTitle == title1);

	// Remove this record in the database
	mDbFieldMeta->DelFieldMeta(fm);
	unsigned long count = mDbFieldMeta->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add one record
	mDbFieldMeta->AddFieldMeta(fm);

	// Add another record
	fm.mTypField = typField2;
	fm.mFieldOrder = fieldOrder2;
	fm.SetFieldId(fieldId2);
	fm.mFieldTitle = title2;
	mDbFieldMeta->AddFieldMeta(fm);

	// Get the first record
	fm.Clear();
	fm.mTypField = typField1;
	fm.SetFieldId(fieldId1);
	isOk = mDbFieldMeta->GetFieldMeta(fm);
	CPPUNIT_ASSERT_MESSAGE("Get FieldMeta 1 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record FieldMeta 1 content error", fm.mTypField == typField1
		&& fm.mFieldOrder == fieldOrder1 && fm.GetFieldId() == fieldId1
		&& fm.mFieldTitle == title1);

	// Get the second record
	fm.Clear();
	fm.mTypField = typField2;
	fm.SetFieldId(fieldId2);
	isOk = mDbFieldMeta->GetFieldMeta(fm);
	CPPUNIT_ASSERT_MESSAGE("Get FieldMeta 2 error", isOk);
	CPPUNIT_ASSERT_MESSAGE("Record FieldMeta 2 content error", fm.mTypField == typField2
		&& fm.mFieldOrder == fieldOrder2 && fm.GetFieldId() == fieldId2
		&& fm.mFieldTitle == title2);
}
//------------------------------------------------------------------------------

void DbFieldMetaTest::testFieldMetaVector(void)
{
	bool isOk;
	FieldMeta fm;
	eTypField typField1 = eTypFieldAll, typField2 = eTypFieldSearch;
	unsigned int fieldOrder1 = 5, fieldOrder2 = 9;
	// Init ids
	UnicodeString fieldId1 = "FieldId 1";
	UnicodeString fieldId2 = "FieldId 2";
	// Init titles
	UnicodeString title1 = "Title 1";
	UnicodeString title2 = "Title 2";

	// Add one record
	fm.mTypField = typField1;
	fm.mFieldOrder = fieldOrder1;
	fm.SetFieldId(fieldId1);
	fm.mFieldTitle = title1;
	mDbFieldMeta->AddFieldMeta(fm);

	// Add another record
	fm.mTypField = typField2;
	fm.mFieldOrder = fieldOrder2;
	fm.SetFieldId(fieldId2);
	fm.mFieldTitle = title2;
	mDbFieldMeta->AddFieldMeta(fm);

	// Get the vector of all field metadata records from a type
	FieldMetaVector vector;
	isOk = mDbFieldMeta->GetFieldMetaVector(vector, typField1);
	CPPUNIT_ASSERT_MESSAGE("Get field metadata vector", isOk && vector.size() == 1);

	// Get the vector of all field metadata records from any type
	isOk = mDbFieldMeta->GetFieldMetaVector(vector, eTypFieldNone);
	CPPUNIT_ASSERT_MESSAGE("Get field metadata vector", isOk && vector.size() == 2);

	// Loop through the vector
	FieldMetaVector::const_iterator it = vector.begin();
	FieldMetaVector::const_iterator itEnd = vector.end();
	for (; it != itEnd; ++it) {
		const FieldMeta &fieldMeta = (*it);
		if (it == vector.begin()) {
			CPPUNIT_ASSERT_MESSAGE("FieldMeta 1 error", fieldMeta.mTypField == typField1
				&& fieldMeta.mFieldOrder == fieldOrder1 && fieldMeta.GetFieldId() == fieldId1
				&& fieldMeta.mFieldTitle == title1);
		}
		else {
			CPPUNIT_ASSERT_MESSAGE("FieldMeta 2 error", fieldMeta.mTypField == typField2
				&& fieldMeta.mFieldOrder == fieldOrder2 && fieldMeta.GetFieldId() == fieldId2
				&& fieldMeta.mFieldTitle == title2);
		}
	}
}
//------------------------------------------------------------------------------
