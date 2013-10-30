/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerItemTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerItemTest.hpp"
#include "UnitTestSuites.hpp"
#include "../indexer/IndexerItem.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(IndexerItemTest, Atoll::UnitTestSuites::Indexer());

IndexerItemTest::IndexerItemTest()
{
}
//------------------------------------------------------------------------------


IndexerItemTest::~IndexerItemTest()
{
}
//------------------------------------------------------------------------------

void IndexerItemTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void IndexerItemTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void IndexerItemTest::testAddWordEntry(void)
{
	IndexerItem *indexerItem;
	std::string indexName("indexer_item_test");
	bool isLowerCase = true;

	indexerItem = new IndexerItem(indexName, eTypIndexWord, isLowerCase);

	// Add words in the set with their position
	// Nothing is written in database
	WordEntry wordEntry;
	wordEntry.mWrd = "Test";
	wordEntry.mEntry.Set(1, 1, 1);
	indexerItem->IiAddWordEntry(wordEntry);
	wordEntry.mWrd = "word";
	wordEntry.mEntry.Set(1, 1, 2);
	indexerItem->IiAddWordEntry(wordEntry);
	wordEntry.mWrd = "word";
	wordEntry.mEntry.Set(1, 1, 3);
	indexerItem->IiAddWordEntry(wordEntry);

	// Get the word entries set
	WordEntriesSet *wordEntriesSet = indexerItem->GetWordEntriesSetToIndex();
	size_t count = wordEntriesSet->size();
  CPPUNIT_ASSERT(count == 2);

	// Clear the set before destroying the item indexer
	// (otherwise a log error is generated)
	wordEntriesSet->clear();

	delete indexerItem;
}
//------------------------------------------------------------------------------
