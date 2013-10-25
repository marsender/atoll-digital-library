/*******************************************************************************

 ExampleTestCase.cpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#include <cppunit/config/SourcePrefix.h>
#include "ExampleTestCase.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(ExampleTestCase);

void ExampleTestCase::setUp()
{
  m_value1 = 2.0;
  m_value2 = 3.0;
}

void ExampleTestCase::testAdd()
{
  double result = m_value1 + m_value2;
  CPPUNIT_ASSERT(result == 5.0);
}


void ExampleTestCase::testEquals()
{
  long l1 = 12;
  long l2 = 12;

  CPPUNIT_ASSERT(l1 == l2);
  CPPUNIT_ASSERT_EQUAL(l1, l2);
}

void ExampleTestCase::testExample()
{
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, 1.1, 0.2); // Expected, actual, delta
}
