/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearcherTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "SearcherTest.hpp"
#include "UnitTestSuites.hpp"
#include "../collection/Collection.hpp"
#include "../engine/EngineEnv.hpp"
//#include "../database/DbManager.hpp"
#include "../database/DbNdx.hpp"
#include "../searcher/Searcher.hpp"
//#include "../lucene/Lucene.hpp"
//#include "../lucene/search/TermQuery.hpp"
#include "unicode/ustring.h" // u_strstr, u_strchr...
#include <vector>
//------------------------------------------------------------------------------

static bool gInitString = false;
U_STRING_DECL(cStrField, "DbNdxTest", 9);

using namespace Atoll;
using namespace Common;
//using namespace Lucene::search;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(SearcherTest, Atoll::UnitTestSuites::Query());

SearcherTest::SearcherTest()
{
	if (!gInitString) {
		U_STRING_INIT(cStrField, "DbNdxTest", 9);
		gInitString = true;
	}
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


SearcherTest::~SearcherTest()
{
}
//------------------------------------------------------------------------------

void SearcherTest::setUp(void)
{
	mEngineEnv = new EngineEnv;
	if (!mEngineEnv->CreateDbEnv(Atoll::UnitTestSuites::GetDbHome())) {
		delete mEngineEnv;
		CPPUNIT_ASSERT_MESSAGE("Create Db env error", false);
	}
	mEngineEnv->SetColId(Atoll::UnitTestSuites::GetColId());

	// Create collection
	mCollection = new Collection(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());

	// Remove all documents and index
	mCollection->ClearCollection();

	// Create the searcher
	mSearcher = new Searcher(mCollection);

	// Create and open the index database
	std::string dbName("DbNdxTest");
	mCollection->AddIndexDatabase(dbName, false);
	mDbNdx = mCollection->GetIndexDatabase(dbName);
	CPPUNIT_ASSERT_MESSAGE("Create database error", mDbNdx);

	/*
	// Create the database manager
	mDbMgr = new DbManager(mEngineEnv->GetDbEnv(), mEngineEnv->GetColId());
	mDbMgr->SetClearOnOpen(true);
	mDbMgr->SetRemoveOnClose(true);

	// Open database
	std::string dbName("DbNdxTest");
	mDbNdx = mDbMgr->GetDbNdx(dbName);

	// Clear the database
	mDbNdx->clear();

	// Create the searcher
	mSearcher = new Searcher();
	mSearcher->SetDbNdx(mDbNdx);
	*/
}
//------------------------------------------------------------------------------

void SearcherTest::tearDown(void)
{
	delete mSearcher;
	//delete mDbMgr;
	delete mCollection;
	delete mEngineEnv;
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherTerm(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherFullField(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "A full field";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database for a field only
	// This will use Lexer::ReadFullField as the field is in <quotes>
	strQuery = "<A full field>";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new entry to a word
	e.Set(2, 2, 2);
	word = "test";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database for a field or a word
	strQuery = "<A full field> test";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherPrefix(void)
{
	bool isLogError;
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = cStrField;
	strQuery += ":test";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Search with a wrong prefix
	isLogError = gLog.isInLogLevel(eTypLogError);
	gLog.removeLogLevel(eTypLogError); // Remove errors
	entrySet.clear();
	strQuery = "wrong_prefix:test";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
	strQuery = "wrong_prefix:test*";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
	if (isLogError)
		gLog.addLogLevel(eTypLogError); // Show errors
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherOr(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(2, 2, 2);
	word = "test2";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test1 test2";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(3, 3, 3);
	word = "test3";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test1 test2 test3";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherAnd(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	entrySet.insert(e);
	word = "test2";
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test1 AND test2";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 3);
	entrySet.insert(e);
	word = "test3";
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test1 AND test2 AND test3";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherPhrase(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	// Add one word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	word = "test2";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2\"";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 3);
	word = "test3";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 5);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2 test3\"";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 4);
	word = "test1";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2 test3 test1\"";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	entrySet.clear();
	e.Set(1, 1, 50);
	word = "test4";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2 test3 test4\"";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherPhraseSlop(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	// Add one word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	word = "test2";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2\"~2";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 3);
	word = "test3";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 5);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2 test3\"~3";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Search the database
	e.Set(1, 1, 5);
	entrySet.insert(e);
	strQuery = "\"test1 test2 test3\"~10";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 4);
	word = "test4";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test4 test3 test2 test1\"~4";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	//entrySet.clear();
	e.Set(1, 1, 50);
	word = "test4";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "\"test1 test2 test3 test4\"~40";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherWildcard(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word;
	// Add one word and entry
	word = "test";
	e.Set(1, 1, 1);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add one word and entry
	word = "testa";
	e.Set(1, 1, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add one word and entry
	word = "testb";
	e.Set(1, 1, 3);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test*";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 2);
	word = "tes";
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test*";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 3);
	entrySet.insert(e);
	word = "test3";
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test*";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Test with accents
	entrySet.clear();
	// Add a new word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	word = "bateaux";
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	entrySet.insert(e);
	UChar s1[] = { 'b', 0x00E2, 't', 'i', 0 }; // "bâti"
	word = s1;
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 3);
	entrySet.insert(e);
	word = "bats";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "bat*";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT_MESSAGE("Wildcard query with accents", entrySet == *(mSearcher->GetEntrySet()));

	// Test with wildcard in the middle
	entrySet.clear();
	// Add a new word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	UChar s2[] = { 'b', 0x00E9, 'n', 'e', 'd', 'i', 'c', 't', 'i', 'o', 'n', 0 }; // "bénediction"
	word = s2;
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	UChar s3[] = { 'b', 0x00E9, 'n', 0x00E9, 'f', 'i', 'c', 'e', 0 }; // "bénéfice"
	word = s3;
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 3);
	entrySet.insert(e);
	word = "benjamin";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "ben*n";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT_MESSAGE("Wildcard query with accents", entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherFuzzy(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 0);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "test~";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	entrySet.clear();
	// Add a new word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	word = "bate";
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	entrySet.insert(e);
	word = "fate";
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 3);
	entrySet.insert(e);
	word = "tate";
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 4);
	entrySet.insert(e);
	word = "tatu";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "tate~";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 5);
	word = "baty";
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "tate~";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherRange(void)
{
	UnicodeString strQuery;
	const UChar *uStr;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word;
	// Add one word and entry
	e.Set(1, 1, 1);
	word = "a";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(2, 2, 2);
	word = "b";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(3, 3, 3);
	word = "c";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);

	// Search the database
	strQuery = "[a TO c]";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT_MESSAGE("[a TO c]", entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(4, 4, 4);
	word = "d";
	mDbNdx->AddWordEntry(word, e);

	// Search the database (without "TO")
	strQuery = "[a c]";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT_MESSAGE("[a c]", entrySet == *(mSearcher->GetEntrySet()));

	// Keep only the "b" in the result set
	entrySet.clear();
	e.Set(2, 2, 2);
	entrySet.insert(e);

	// Search the database
	strQuery = "{a TO c}";
	uStr = strQuery.getTerminatedBuffer();
	mSearcher->SetQuery(cStrField, uStr);
	mSearcher->Compute();
	CPPUNIT_ASSERT_MESSAGE("{a TO c}", entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

/*
void SearcherTest::testSearcherFolowing(void)
{
	UnicodeString strQuery;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	// Add one word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 3);
	word = "test2";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 5);
	word = "test3";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 5);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	mDbNdx->AddWordEntry(word, e);

	// Create the query
	Xapian::Query *query;
	std::vector<Xapian::Query> subqs;
	subqs.push_back(Xapian::Query("test1"));
	subqs.push_back(Xapian::Query("test2"));
	subqs.push_back(Xapian::Query("test3"));
	query = new Xapian::Query(Xapian::Query::OP_PHRASE, subqs.begin(), subqs.end(), 2);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Create the query with the terms in reverse order
	entrySet.clear();
	subqs.clear();
	subqs.push_back(Xapian::Query("test2"));
	subqs.push_back(Xapian::Query("test1"));
	query = new Xapian::Query(Xapian::Query::OP_PHRASE, subqs.begin(), subqs.end(), 2);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------

void SearcherTest::testSearcherNear(void)
{
	UnicodeString strQuery;

	// Populate the database
	Entry e;
	EntrySet entrySet;
	UnicodeString word = "test1";
	// Add one word and entry
	e.Set(1, 1, 1);
	entrySet.insert(e);
	// Add one word and entry
	mDbNdx->AddWordEntry(word, e);
	// Add a new word and entry
	e.Set(1, 1, 2);
	word = "test2";
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 3);
	entrySet.insert(e);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(2, 2, 2);
	mDbNdx->AddWordEntry(word, e);

	// Create the query
	Xapian::Query *query;
	std::vector<Xapian::Query> subqs;
	subqs.push_back(Xapian::Query("test1"));
	subqs.push_back(Xapian::Query("test2"));
	query = new Xapian::Query(Xapian::Query::OP_NEAR, subqs.begin(), subqs.end(), 6);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Create the query with the terms in reverse order
	subqs.clear();
	subqs.push_back(Xapian::Query("test2"));
	subqs.push_back(Xapian::Query("test1"));
	query = new Xapian::Query(Xapian::Query::OP_NEAR, subqs.begin(), subqs.end(), 6);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	e.Set(1, 1, 7);
	entrySet.insert(e);
	word = "test3";
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(1, 1, 10);
	mDbNdx->AddWordEntry(word, e);
	// Add a new entry to this word
	e.Set(3, 3, 3);
	mDbNdx->AddWordEntry(word, e);

	// Create the query
	subqs.clear();
	subqs.push_back(Xapian::Query("test1"));
	subqs.push_back(Xapian::Query("test2"));
	subqs.push_back(Xapian::Query("test3"));
	query = new Xapian::Query(Xapian::Query::OP_NEAR, subqs.begin(), subqs.end(), 6);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));

	// Add a new word and entry
	entrySet.clear();
	e.Set(1, 1, 50);
	word = "test4";
	mDbNdx->AddWordEntry(word, e);
	// Create the query
	subqs.clear();
	subqs.push_back(Xapian::Query("test1"));
	subqs.push_back(Xapian::Query("test2"));
	subqs.push_back(Xapian::Query("test3"));
	subqs.push_back(Xapian::Query("test4"));
	query = new Xapian::Query(Xapian::Query::OP_NEAR, subqs.begin(), subqs.end(), 6);
	// Search the database
	mSearcher->SetQuery(cStrField, query);
	mSearcher->Compute();
	CPPUNIT_ASSERT(entrySet == *(mSearcher->GetEntrySet()));
}
//------------------------------------------------------------------------------
*/
