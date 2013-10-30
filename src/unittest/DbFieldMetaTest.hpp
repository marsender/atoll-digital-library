/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbFieldMetaTest.hpp

*******************************************************************************/

#ifndef __DbFieldMetaTest_HPP
#define	__DbFieldMetaTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbFieldMeta;
}

//! DbFieldMeta test class
class DbFieldMetaTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbFieldMetaTest);
  CPPUNIT_TEST(testAddFieldMeta);
  CPPUNIT_TEST(testFieldMetaVector);
  CPPUNIT_TEST_SUITE_END();

public:
  DbFieldMetaTest();
  virtual ~DbFieldMetaTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddFieldMeta(void);
  void testFieldMetaVector(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbFieldMeta *mDbFieldMeta;

	// No need for copy and assignment
	DbFieldMetaTest(const DbFieldMetaTest &);
	DbFieldMetaTest &operator=(const DbFieldMetaTest &);
};
//------------------------------------------------------------------------------

#endif
