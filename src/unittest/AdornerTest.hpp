/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerTest.hpp

*******************************************************************************/

#ifndef __AdornerTest_HPP
#define	__AdornerTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Adorner test class
class AdornerTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(AdornerTest);
  CPPUNIT_TEST(testAdornContent);
  CPPUNIT_TEST_SUITE_END();

public:
  AdornerTest();
  virtual ~AdornerTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test nodes and characters adornation
  void testAdornContent(void);

private:
	std::string mPath;

	// No need for copy and assignment
	AdornerTest(const AdornerTest &);
	AdornerTest &operator=(const AdornerTest &);
};
//------------------------------------------------------------------------------

#endif
