/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesTest.hpp

*******************************************************************************/

#ifndef __XercesTest_HPP
#define	__XercesTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Xerces parser test class
class XercesTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(XercesTest);
  CPPUNIT_TEST(testXercesString);
	CPPUNIT_TEST(testXercesCatalogResolver);
  CPPUNIT_TEST_SUITE_END();

public:
  XercesTest();
  virtual ~XercesTest();

  void setUp();
  void tearDown();

protected:
  void testXercesString();
	void testXercesCatalogResolver();

private:
	// No need for copy and assignment
	XercesTest(const XercesTest &);
	XercesTest &operator=(const XercesTest &);
};
//------------------------------------------------------------------------------

#endif
