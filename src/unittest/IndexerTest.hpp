/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerTest.hpp

*******************************************************************************/

#ifndef __IndexerTest_HPP
#define	__IndexerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
}

//! Indexer test class
class IndexerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(IndexerTest);
  CPPUNIT_TEST(testIndexContent);
  CPPUNIT_TEST_SUITE_END();

public:
  IndexerTest();
  virtual ~IndexerTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test nodes and characters indexation
  void testIndexContent(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;

	// No need for copy and assignment
	IndexerTest(const IndexerTest &);
	IndexerTest &operator=(const IndexerTest &);
};
//------------------------------------------------------------------------------

#endif
