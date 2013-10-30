/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbMapTest.hpp

*******************************************************************************/

#ifndef __DbMapTest_HPP
#define	__DbMapTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbMap;
}

//! DbMap test class
class DbMapTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbMapTest);
  CPPUNIT_TEST(testUnicodeStringToIntMap);
  CPPUNIT_TEST_SUITE_END();

public:
  DbMapTest();
  virtual ~DbMapTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testUnicodeStringToIntMap(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbMap *mDbMap;

	// No need for copy and assignment
	DbMapTest(const DbMapTest &);
	DbMapTest &operator=(const DbMapTest &);
};
//------------------------------------------------------------------------------

#endif
