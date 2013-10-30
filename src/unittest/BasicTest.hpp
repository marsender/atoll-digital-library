/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

BasicTest.hpp

*******************************************************************************/

#ifndef __BasicTest_HPP
#define	__BasicTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Basic test class
class BasicTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(BasicTest);
  CPPUNIT_TEST(testBasic);
  CPPUNIT_TEST_SUITE_END();

public:
  BasicTest();
  virtual ~BasicTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Basic unit test
  void testBasic(void);

private:
	// No need for copy and assignment
	BasicTest(const BasicTest &);
	BasicTest &operator=(const BasicTest &);
};
//------------------------------------------------------------------------------

#endif
