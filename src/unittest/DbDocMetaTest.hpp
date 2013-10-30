/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbDocTest.hpp

*******************************************************************************/

#ifndef __DbDocTest_HPP
#define	__DbDocTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbDocMeta;
}

//! DbDocMeta test class
class DbDocMetaTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbDocMetaTest);
  CPPUNIT_TEST(testAddDocMeta);
  CPPUNIT_TEST(testDocMetaVector);
  CPPUNIT_TEST_SUITE_END();

public:
  DbDocMetaTest();
  virtual ~DbDocMetaTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddDocMeta(void);
  void testDocMetaVector(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbDocMeta *mDbDocMeta;

	// No need for copy and assignment
	DbDocMetaTest(const DbDocMetaTest &);
	DbDocMetaTest &operator=(const DbDocMetaTest &);
};
//------------------------------------------------------------------------------

#endif
