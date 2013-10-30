/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DocMetaTest.hpp

*******************************************************************************/

#ifndef __DocMetaTest_HPP
#define	__DocMetaTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! DocMeta test class
class DocMetaTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(DocMetaTest);
  CPPUNIT_TEST(testSerialise);
  CPPUNIT_TEST_SUITE_END();

public:
  DocMetaTest();
  virtual ~DocMetaTest();

	//! Setup called before each test
  void setUp(void);
	//! Shutdown called after each test
  void tearDown(void);

protected:
	//! Test serialisation
  void testSerialise(void);

private:
	std::string mPath;

	// No need for copy and assignment
	DocMetaTest(const DocMetaTest &);
	DocMetaTest &operator=(const DocMetaTest &);
};
//------------------------------------------------------------------------------

#endif
