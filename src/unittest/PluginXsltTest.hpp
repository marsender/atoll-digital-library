/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXsltTest.hpp

*******************************************************************************/

#ifndef __PluginXsltTest_HPP
#define	__PluginXsltTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Plugin Xslt test class
class PluginXsltTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(PluginXsltTest);
  CPPUNIT_TEST(testXsltBuffer);
  CPPUNIT_TEST(testXsltCompiled);
  CPPUNIT_TEST_SUITE_END();

public:
  PluginXsltTest();
  virtual ~PluginXsltTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testXsltBuffer(void);
  void testXsltCompiled(void);

private:
	std::string mPath;

	// No need for copy and assignment
	PluginXsltTest(const PluginXsltTest &);
	PluginXsltTest &operator=(const PluginXsltTest &);
	//! Run the test on one stylesheet
  void runXsltCompiled(const char *inFile);
};
//------------------------------------------------------------------------------

#endif
