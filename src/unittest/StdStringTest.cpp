/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

StdStringTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "StdStringTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/StdString.hpp"
//------------------------------------------------------------------------------

using namespace std;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(StdStringTest, Atoll::UnitTestSuites::Common());

StdStringTest::StdStringTest()
{
}
//------------------------------------------------------------------------------


StdStringTest::~StdStringTest()
{
}
//------------------------------------------------------------------------------

void StdStringTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void StdStringTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void StdStringTest::testFindAndReplace(void)
{
	string base;

	base = "this is a test string.";
	StdString::FindAndReplace(base, " ", "-");
	CPPUNIT_ASSERT_MESSAGE("Incorrect find / replace", base == "this-is-a-test-string.");

	base = "--";
	StdString::FindAndReplace(base, "-", "--");
	CPPUNIT_ASSERT_MESSAGE("Incorrect find / replace", base == "----");

	base = "--";
	StdString::FindAndReplace(base, "-", "");
	CPPUNIT_ASSERT_MESSAGE("Incorrect find / replace", base == "");
}
//------------------------------------------------------------------------------

void StdStringTest::testLowercase(void)
{
	string base;

	base = "THIS IS A TEST STRING.";
	StdString::ToLower(base);
	CPPUNIT_ASSERT_MESSAGE("Incorrect lowercase", base == "this is a test string.");
}
//------------------------------------------------------------------------------
