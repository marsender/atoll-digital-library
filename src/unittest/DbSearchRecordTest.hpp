/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbSearchRecordTest.hpp

*******************************************************************************/

#ifndef __DbSearchRecordTest_HPP
#define	__DbSearchRecordTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbSearchRecord;
}

//! DbSearchRecord test class
class DbSearchRecordTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbSearchRecordTest);
  CPPUNIT_TEST(testAddSearchRecord);
  CPPUNIT_TEST(testSearchRecordVector);
  CPPUNIT_TEST_SUITE_END();

public:
  DbSearchRecordTest();
  virtual ~DbSearchRecordTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddSearchRecord(void);
  void testSearchRecordVector(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbSearchRecord *mDbSearchRecord;

	// No need for copy and assignment
	DbSearchRecordTest(const DbSearchRecordTest &);
	DbSearchRecordTest &operator=(const DbSearchRecordTest &);
};
//------------------------------------------------------------------------------

#endif
