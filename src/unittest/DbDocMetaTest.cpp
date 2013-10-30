/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbDocMetaTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbDocMetaTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbDocMeta.hpp"
#include "../database/DbManager.hpp"
#include "../util/DocMeta.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbDocMetaTest, Atoll::UnitTestSuites::Database());

DbDocMetaTest::DbDocMetaTest()
{
}
//------------------------------------------------------------------------------

DbDocMetaTest::~DbDocMetaTest()
{
}
//------------------------------------------------------------------------------

void DbDocMetaTest::setUp(void)
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
	UnicodeString dbName("DbDocMetaTest");
	mDbDocMeta = mDbMgr->GetDbDocMeta(dbName);

	// Clear the database
	mDbDocMeta->clear();
}
//------------------------------------------------------------------------------

void DbDocMetaTest::tearDown(void)
{
	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbDocMetaTest::testAddDocMeta(void)
{
	unsigned long docNum1 = 1, docNum2 = 2;
	// Init description (plain text)
	std::string fileName("/test/filename.xml");
	std::string dcMetadata("/test/filename_dcmetadata.xml");
	UnicodeString texte1("Text content 1");
	UnicodeString texte2("Text content 2");

	DocMeta dm;
	dm.mDocNum = docNum1;
	dm.mDocTitle = texte1;
	dm.mFileName = fileName;
	dm.mDcMetadata = dcMetadata;

	// Add one record
	mDbDocMeta->AddDocMeta(dm);
	// Add same record
	mDbDocMeta->AddDocMeta(dm);

	// Get this record
	dm.Clear();
	dm.mDocNum = docNum1;
	mDbDocMeta->GetDocMeta(dm);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta DocNum error", dm.mDocNum == docNum1);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta DocTitle error", dm.mDocTitle == texte1);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta FileName error", dm.mFileName == fileName);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta DcMetadata error", dm.mDcMetadata == dcMetadata);

	// Remove this record in the database
	mDbDocMeta->DelDocMeta(docNum1);
	unsigned long count = mDbDocMeta->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add one record
	mDbDocMeta->AddDocMeta(dm);

	// Add another record
	dm.mDocNum = docNum2;
	dm.mDocTitle = texte2;
	mDbDocMeta->AddDocMeta(dm);

	// Get the first record
	dm.Clear();
	dm.mDocNum = docNum1;
	mDbDocMeta->GetDocMeta(dm);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta 1 error", dm.mDocNum == docNum1 && dm.mDocTitle == texte1);

	// Get the second record
	dm.Clear();
	dm.mDocNum = docNum2;
	mDbDocMeta->GetDocMeta(dm);
	CPPUNIT_ASSERT_MESSAGE("Get DocMeta 2 error", dm.mDocNum == docNum2 && dm.mDocTitle == texte2);
}
//------------------------------------------------------------------------------

void DbDocMetaTest::testDocMetaVector(void)
{
	bool isOk;
	DocMeta dm;
	unsigned long docNum1 = 1, docNum2 = 2;
	// Init description (plain text)
	UnicodeString texte1("Text content 1");
	UnicodeString texte2("Text content 2");

	// Add one record
	dm.mDocNum = docNum1;
	dm.mDocTitle = texte1;
	mDbDocMeta->AddDocMeta(dm);

	// Add another record
	dm.mDocNum = docNum2;
	dm.mDocTitle = texte2;
	mDbDocMeta->AddDocMeta(dm);

	// Get the vector of all document metadata records
	DocMetaVector vector;
	isOk = mDbDocMeta->GetDocMetaVector(vector);
	CPPUNIT_ASSERT_MESSAGE("Get document metadata vector", isOk && vector.size() == 2);

	// Loop through the vector
	DocMetaVector::const_iterator it = vector.begin();
	DocMetaVector::const_iterator itEnd = vector.end();
	for (; it != itEnd; ++it) {
		const DocMeta &docMeta = (*it);
		if (it == vector.begin()) {
			CPPUNIT_ASSERT_MESSAGE("DocMeta 1 error", docMeta.mDocNum == docNum1
				&& docMeta.mDocTitle == texte1);
		}
		else {
			CPPUNIT_ASSERT_MESSAGE("DocMeta 2 error", docMeta.mDocNum == docNum2
				&& docMeta.mDocTitle == texte2);
		}
	}
}
//------------------------------------------------------------------------------
