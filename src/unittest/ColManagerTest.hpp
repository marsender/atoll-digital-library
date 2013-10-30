/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColManagerTest.hpp

*******************************************************************************/

#ifndef __ColManagerTest_HPP
#define	__ColManagerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class ColManager;
}

//! ColManager test class
class ColManagerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ColManagerTest);
  CPPUNIT_TEST(testAddRemoveCollection);
  CPPUNIT_TEST(testOpenCloseCollection);
  CPPUNIT_TEST_SUITE_END();

public:
  ColManagerTest();
  virtual ~ColManagerTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testAddRemoveCollection(void);
  void testOpenCloseCollection(void);

private:
	Atoll::EngineEnv *mEngineEnv;
	Atoll::ColManager *mColManager;

	// No need for copy and assignment
	ColManagerTest(const ColManagerTest &);
	ColManagerTest &operator=(const ColManagerTest &);
};
//------------------------------------------------------------------------------

#endif
