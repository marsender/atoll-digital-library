/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXmlExecTest.hpp

*******************************************************************************/

#ifndef __PluginXmlExecTest_HPP
#define	__PluginXmlExecTest_HPP
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

//! Plugin XmlExec test class
class PluginXmlExecTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(PluginXmlExecTest);
  CPPUNIT_TEST(testXmlExecBuffer);
  CPPUNIT_TEST(testAtollXmlExec);
  CPPUNIT_TEST_SUITE_END();

public:
  PluginXmlExecTest();
  virtual ~PluginXmlExecTest();

  void setUp(void);
  void tearDown(void);

protected:
  void testXmlExecBuffer(void);
	void testAtollXmlExec(void);

private:
	// No need for copy and assignment
	PluginXmlExecTest(const PluginXmlExecTest &);
	PluginXmlExecTest &operator=(const PluginXmlExecTest &);
};
//------------------------------------------------------------------------------

#endif
