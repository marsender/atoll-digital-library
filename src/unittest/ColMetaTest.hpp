/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColMetaTest.hpp

*******************************************************************************/

#ifndef __ColMetaTest_HPP
#define	__ColMetaTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! ColMeta test class
class ColMetaTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ColMetaTest);
  CPPUNIT_TEST(testSerialise);
  CPPUNIT_TEST_SUITE_END();

public:
  ColMetaTest();
  virtual ~ColMetaTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test nodes and characters adornation
  void testSerialise(void);

private:
	// No need for copy and assignment
	ColMetaTest(const ColMetaTest &);
	ColMetaTest &operator=(const ColMetaTest &);
};
//------------------------------------------------------------------------------

#endif
