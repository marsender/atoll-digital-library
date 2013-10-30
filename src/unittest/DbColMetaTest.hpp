/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbColTest.hpp

*******************************************************************************/

#ifndef __DbColTest_HPP
#define	__DbColTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbColMeta;
}

//! DbColMeta test class
class DbColMetaTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbColMetaTest);
  CPPUNIT_TEST(testAddColMeta);
  CPPUNIT_TEST(testColMetaVector);
  CPPUNIT_TEST_SUITE_END();

public:
  DbColMetaTest();
  virtual ~DbColMetaTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddColMeta(void);
  void testColMetaVector(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbColMeta *mDbColMeta;

	// No need for copy and assignment
	DbColMetaTest(const DbColMetaTest &);
	DbColMetaTest &operator=(const DbColMetaTest &);
};
//------------------------------------------------------------------------------

#endif
