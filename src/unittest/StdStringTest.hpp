/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

StdStringTest.hpp

*******************************************************************************/

#ifndef __StdStringTest_HPP
#define	__StdStringTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! StdString test class
class StdStringTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(StdStringTest);
  CPPUNIT_TEST(testFindAndReplace);
  CPPUNIT_TEST(testLowercase);
  CPPUNIT_TEST_SUITE_END();

public:
  StdStringTest();
  virtual ~StdStringTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Find / replace test
  void testFindAndReplace(void);
	//! Lowercase test
  void testLowercase(void);

private:
	// No need for copy and assignment
	StdStringTest(const StdStringTest &);
	StdStringTest &operator=(const StdStringTest &);
};
//------------------------------------------------------------------------------

#endif
