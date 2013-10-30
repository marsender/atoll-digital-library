/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EnumStringTest.hpp

*******************************************************************************/

#ifndef __EnumStringTest_HPP
#define	__EnumStringTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! EnumString test class
class EnumStringTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(EnumStringTest);
  CPPUNIT_TEST(testEnumeration);
  CPPUNIT_TEST_SUITE_END();

public:
  EnumStringTest();
  virtual ~EnumStringTest();

	//! Setup called before each test
  void setUp();
	//! Shutdown called after each test
  void tearDown();

protected:
  void testEnumeration();

private:
	// No need for copy and assignment
	EnumStringTest(const EnumStringTest &);
	EnumStringTest &operator=(const EnumStringTest &);
};
//------------------------------------------------------------------------------

#endif
