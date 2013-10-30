/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerDecoratorTest.hpp

*******************************************************************************/

#ifndef __AdornerDecoratorTest_HPP
#define	__AdornerDecoratorTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! AdornerDecorator test class
class AdornerDecoratorTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(AdornerDecoratorTest);
  CPPUNIT_TEST(testDecorator);
  CPPUNIT_TEST_SUITE_END();

public:
  AdornerDecoratorTest();
  virtual ~AdornerDecoratorTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test nodes and characters adornation
  void testDecorator(void);

private:
	// No need for copy and assignment
	AdornerDecoratorTest(const AdornerDecoratorTest &);
	AdornerDecoratorTest &operator=(const AdornerDecoratorTest &);
};
//------------------------------------------------------------------------------

#endif
