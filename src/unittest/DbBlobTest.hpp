/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbBlobTest.hpp

*******************************************************************************/

#ifndef __DbBlobTest_HPP
#define	__DbBlobTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbBlob;
}

//! DbBlob test class
class DbBlobTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbBlobTest);
  CPPUNIT_TEST(testMain);
  CPPUNIT_TEST_SUITE_END();

public:
  DbBlobTest();
  virtual ~DbBlobTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testMain(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbBlob *mDbBlob;

	// No need for copy and assignment
	DbBlobTest(const DbBlobTest &);
	DbBlobTest &operator=(const DbBlobTest &);
};
//------------------------------------------------------------------------------

#endif
