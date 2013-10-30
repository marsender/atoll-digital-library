/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocRecordTest.hpp

*******************************************************************************/

#ifndef __DocRecordTest_HPP
#define	__DocRecordTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! DocRecord test class
class DocRecordTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DocRecordTest);
  CPPUNIT_TEST(testSerialise);
  CPPUNIT_TEST_SUITE_END();

public:
  DocRecordTest();
  virtual ~DocRecordTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test serialisation
  void testSerialise(void);

private:
	// No need for copy and assignment
	DocRecordTest(const DocRecordTest &);
	DocRecordTest &operator=(const DocRecordTest &);
};
//------------------------------------------------------------------------------

#endif
