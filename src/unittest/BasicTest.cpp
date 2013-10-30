/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

BasicTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "BasicTest.hpp"
#include "UnitTestSuites.hpp"
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(BasicTest, Atoll::UnitTestSuites::Core());

BasicTest::BasicTest()
{
}
//------------------------------------------------------------------------------


BasicTest::~BasicTest()
{
}
//------------------------------------------------------------------------------

void BasicTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void BasicTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void BasicTest::testBasic(void)
{
	int i = 11;

  CPPUNIT_ASSERT(i == 11);
  CPPUNIT_ASSERT_EQUAL(i, 11);
	CPPUNIT_ASSERT_MESSAGE("Incorrect i value", i == 11);
	//CPPUNIT_ASSERT_THROW(assertion, AppException);
}
//------------------------------------------------------------------------------
