/*******************************************************************************

 ExampleTestCase.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef CPP_UNIT_EXAMPLETESTCASE_H
#define CPP_UNIT_EXAMPLETESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

/**
 * A sample test case
 */
class ExampleTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ExampleTestCase);
  CPPUNIT_TEST(testAdd);
  CPPUNIT_TEST(testEquals);
  CPPUNIT_TEST(testExample);
  CPPUNIT_TEST_SUITE_END();

protected:
  double m_value1;
  double m_value2;

public:
  void setUp();

protected:
  void testAdd();
  void testEquals();
  void testExample();
};


#endif
