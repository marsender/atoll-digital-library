/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbNdxTest.hpp

*******************************************************************************/

#ifndef __DbNdxTest_HPP
#define	__DbNdxTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class DbManager;
class DbNdx;
}

//! DbNdx test class
class DbNdxTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DbNdxTest);
  CPPUNIT_TEST(testManualAddWord);
  CPPUNIT_TEST(testLoadFwb);
  CPPUNIT_TEST_SUITE_END();

public:
  DbNdxTest();
  virtual ~DbNdxTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testManualAddWord(void);
  void testLoadFwb(void);

private:
	std::string mPath;
	Atoll::EngineEnv *mEngineEnv;
	Atoll::DbManager *mDbMgr;
	Atoll::DbNdx *mDbNdx;

	// No need for copy and assignment
	DbNdxTest(const DbNdxTest &);
	DbNdxTest &operator=(const DbNdxTest &);
};
//------------------------------------------------------------------------------

#endif
