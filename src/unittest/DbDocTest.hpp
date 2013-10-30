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
class DbDoc;
}

//! DbDoc test class
class DbDocTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbDocTest);
  CPPUNIT_TEST(testAddDocRecord);
  CPPUNIT_TEST(testDocRecordVector);
  CPPUNIT_TEST_SUITE_END();

public:
  DbDocTest();
  virtual ~DbDocTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddDocRecord(void);
  void testDocRecordVector(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbDoc *mDbDoc;

	// No need for copy and assignment
	DbDocTest(const DbDocTest &);
	DbDocTest &operator=(const DbDocTest &);
};
//------------------------------------------------------------------------------

#endif
