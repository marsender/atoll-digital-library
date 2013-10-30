/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbManagerTest.hpp

*******************************************************************************/

#ifndef __DbManagerTest_HPP
#define	__DbManagerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
}

//! DbManager test class
class DbManagerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbManagerTest);
  CPPUNIT_TEST(testDbName);
  CPPUNIT_TEST(testDbColMetaPointer);
  CPPUNIT_TEST(testDbDocMetaPointer);
  CPPUNIT_TEST(testDbDocPointer);
  CPPUNIT_TEST(testDbNdxPointer);
  CPPUNIT_TEST(testDbMapPointer);
  CPPUNIT_TEST(testDbClear);
  CPPUNIT_TEST_SUITE_END();

public:
  DbManagerTest();
  virtual ~DbManagerTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testDbName(void);
  void testDbColMetaPointer(void);
  void testDbDocMetaPointer(void);
  void testDbDocPointer(void);
  void testDbNdxPointer(void);
  void testDbMapPointer(void);
  void testDbClear(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;

	// No need for copy and assignment
	DbManagerTest(const DbManagerTest &);
	DbManagerTest &operator=(const DbManagerTest &);
};
//------------------------------------------------------------------------------

#endif
