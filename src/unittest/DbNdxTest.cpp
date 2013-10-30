/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbNdxTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbNdxTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineEnv.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbNdx.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DbNdxTest, Atoll::UnitTestSuites::Database());

DbNdxTest::DbNdxTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


DbNdxTest::~DbNdxTest()
{
}
//------------------------------------------------------------------------------

void DbNdxTest::setUp(void)
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
	std::string dbName("DbNdxTest");
	mDbNdx = mDbMgr->GetDbNdx(dbName);

	// Clear the database
	mDbNdx->clear();
}
//------------------------------------------------------------------------------

void DbNdxTest::tearDown(void)
{
	// Dump to file
	/*
	std::string file("c:\\temp\\dump.txt");
	std::ofstream out(file.c_str(), std::ios::out);
	//mDbNdx->close(0);
	mDbNdx->dump(&out);
	*/

	delete mDbMgr;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void DbNdxTest::testManualAddWord(void)
{
	Entry e;
	size_t sSize;
	EntrySet entrySet;
	UnicodeString word = "test1";
	UnicodeString word2 = "test2";
	WordIntVector wordIntVector;
	unsigned long min, nb;

	// Insert some words
	e.Set(1, 1, 1);
	// Add one word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add same word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Get this word entries
	mDbNdx->GetWordEntries(word, entrySet);
	sSize = entrySet.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 2);

	// Get the other word entries
	mDbNdx->GetWordEntries(word2, entrySet);
	sSize = entrySet.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 0);

	// Add another word and entry
	mDbNdx->AddWordEntry(word2, e);

	// Get the list of words
	min = 0;
	nb = 100;
	wordIntVector.clear();
	mDbNdx->GetWordIntList(wordIntVector, min, nb);
	sSize = wordIntVector.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 2);

	// Get the list of words from a min
	min = 1;
	wordIntVector.clear();
	mDbNdx->GetWordIntList(wordIntVector, min, nb);
	sSize = wordIntVector.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 1);

	// Remove the second word from the database
	mDbNdx->DelWordEntries(word2);

	// Remove all the entries for this word, which are in one document
	mDbNdx->DelWordDocEntries(word, 2);
	mDbNdx->GetWordEntries(word, entrySet);
	sSize = entrySet.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 1);

	// Remove this word from the database
	mDbNdx->DelWordEntries(word);
	unsigned long count = mDbNdx->Count();
  CPPUNIT_ASSERT_MESSAGE("Count error", count == 0);

	// Add new entries with a set
	entrySet.clear();
	e.Set(1, 1, 1);
	entrySet.insert(e);
	e.Set(2, 2, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntries(word, entrySet);
	mDbNdx->GetWordEntries(word, entrySet);
	sSize = entrySet.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 2);

	// Remove all entries in the database, for one doc
	mDbNdx->DelDocEntries(2);
	mDbNdx->GetWordEntries(word, entrySet);
	sSize = entrySet.size();
  CPPUNIT_ASSERT_MESSAGE("Count error", sSize == 1);
}
//------------------------------------------------------------------------------

void DbNdxTest::testLoadFwb(void)
{
	UnicodeString str;
	unsigned long pos;
	unsigned long count;

	std::string fwbFile = mPath + "test_bal_0167.fwb";
	mDbNdx->LoadFwb(fwbFile);

	count = mDbNdx->Count();
  CPPUNIT_ASSERT(count > 0);

	UnicodeStringVector vector;
	mDbNdx->GetWordList(vector);
  CPPUNIT_ASSERT(vector.size() == count);

	// Get first item and verify it's position
	mDbNdx->GetWord(str, 0);
	pos = mDbNdx->GetWordRecNum(str);
	CPPUNIT_ASSERT(pos == 0);

	// Get second item and verify it's position
	mDbNdx->GetWord(str, 1);
	pos = mDbNdx->GetWordRecNum(str);
	CPPUNIT_ASSERT(pos == 1);

	// Get last item and verify it's position
	bool isWord = mDbNdx->GetWord(str, count - 1);
	CPPUNIT_ASSERT(isWord);

	pos = mDbNdx->GetWordRecNum(str);
	CPPUNIT_ASSERT(pos == count - 1);

	// Next test must give an error,
	// because the last word is at the position "count - 1"
	//CPPUNIT_ASSERT_THROW(mDbNdx->GetWord(str, count), AppException);
}
//------------------------------------------------------------------------------
