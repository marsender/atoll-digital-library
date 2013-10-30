/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerItemTest.hpp

*******************************************************************************/

#ifndef __IndexerItemTest_HPP
#define	__IndexerItemTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! IndexerItem test class
class IndexerItemTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(IndexerItemTest);
  CPPUNIT_TEST(testAddWordEntry);
  CPPUNIT_TEST_SUITE_END();

public:
  IndexerItemTest();
  virtual ~IndexerItemTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Add word entry unit test
  void testAddWordEntry(void);

private:
	// No need for copy and assignment
	IndexerItemTest(const IndexerItemTest &);
	IndexerItemTest &operator=(const IndexerItemTest &);
};
//------------------------------------------------------------------------------

#endif
