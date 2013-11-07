/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearcherTest.hpp

*******************************************************************************/

#ifndef __SearcherTest_HPP
#define	__SearcherTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class Collection;
//class DbManager;
class DbNdx;
class Searcher;
}

//! Query test class
class SearcherTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(SearcherTest);
  CPPUNIT_TEST(testSearcherTerm);
	CPPUNIT_TEST(testSearcherFullField);
  CPPUNIT_TEST(testSearcherPrefix);
  CPPUNIT_TEST(testSearcherOr);
  CPPUNIT_TEST(testSearcherAnd);
  CPPUNIT_TEST(testSearcherPhrase);
  CPPUNIT_TEST(testSearcherPhraseSlop);
  CPPUNIT_TEST(testSearcherWildcard);
  CPPUNIT_TEST(testSearcherFuzzy);
  CPPUNIT_TEST(testSearcherRange);
  //CPPUNIT_TEST(testSearcherFolowing);
  //CPPUNIT_TEST(testSearcherNear);
  CPPUNIT_TEST_SUITE_END();

public:
  SearcherTest();
  virtual ~SearcherTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Search a term
  void testSearcherTerm(void);
	//! Search a full field
  void testSearcherFullField(void);
	//! Search a term with prefix
  void testSearcherPrefix(void);
	//! Search OR query between terms
  void testSearcherOr(void);
	//! Search AND query between terms
  void testSearcherAnd(void);
	//! Search PHRASE query between terms
  void testSearcherPhrase(void);
	//! Search PHRASE SLOP query between terms
  void testSearcherPhraseSlop(void);
	//! Search Wildcard term
  void testSearcherWildcard(void);
	//! Search Fuzzy term
  void testSearcherFuzzy(void);
	//! Search Range term
  void testSearcherRange(void);
	//! Search Folowing terms
  //void testSearcherFolowing(void);
	//! Search NEAR query between terms
  //void testSearcherNear(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::Collection *mCollection;
	//Atoll::DbManager *mDbMgr;
	Atoll::DbNdx *mDbNdx;
	Atoll::Searcher *mSearcher;

	// No need for copy and assignment
	SearcherTest(const SearcherTest &);
	SearcherTest &operator=(const SearcherTest &);
};
//------------------------------------------------------------------------------

#endif
