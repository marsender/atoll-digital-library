/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerHandlerTest.hpp

*******************************************************************************/

#ifndef __IndexerHandlerTest_HPP
#define	__IndexerHandlerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class Parser;
class IndexerHandler;
}

//! Xerces IndexerHandler test class
class IndexerHandlerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(IndexerHandlerTest);
  CPPUNIT_TEST(testIndexerHandler);
  CPPUNIT_TEST_SUITE_END();

public:
  IndexerHandlerTest();
  virtual ~IndexerHandlerTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testIndexerHandler(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::Parser *mParser;
	Atoll::IndexerHandler *mIndexerHandler;

	void CheckDatabaseContent(const char *inDbName, unsigned long inDbWrdCount,
		const char *inWord, const char *inStrEntrySet);

	// No need for copy and assignment
	IndexerHandlerTest(const IndexerHandlerTest &);
	IndexerHandlerTest &operator=(const IndexerHandlerTest &);
};
//------------------------------------------------------------------------------

#endif
