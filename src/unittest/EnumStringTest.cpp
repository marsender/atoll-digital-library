/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EnumStringTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "EnumStringTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/EnumString.hpp"
//------------------------------------------------------------------------------

namespace Common
{
	// Enumeration
	enum eWeekEnd {
		eSaturday = 6,
		eSunday = 7
	};

	// String support for enumeration
	EnumStringBegin(eWeekEnd) {
		RegisterEnumerator(eSaturday, "Saturday"); // EnumStringAdd(Saturday);
		RegisterEnumerator(eSunday, "Sunday"); // EnumStringAdd(Sunday);
	}
	EnumStringEnd;
} // namespace Common

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(EnumStringTest, Atoll::UnitTestSuites::Common());

EnumStringTest::EnumStringTest()
{
}
//------------------------------------------------------------------------------


EnumStringTest::~EnumStringTest()
{
}
//------------------------------------------------------------------------------

void EnumStringTest::setUp()
{
}
//------------------------------------------------------------------------------

void EnumStringTest::tearDown()
{
}
//------------------------------------------------------------------------------

void EnumStringTest::testEnumeration()
{
	// Convert from enum to string
	const std::string &str = Common::EnumString<Common::eWeekEnd>::From(Common::eSaturday);
	// str should now be "Saturday"
	CPPUNIT_ASSERT(str.compare("Saturday") == 0);

	// Convert from string to enum
	Common::eWeekEnd w = Common::eSaturday;
	const bool isOk = Common::EnumString<Common::eWeekEnd>::To(w, "Sunday"); // or To(w, str);
	// w should now be Sunday
	CPPUNIT_ASSERT(isOk);
	CPPUNIT_ASSERT(w == Common::eSunday);
}
//------------------------------------------------------------------------------
