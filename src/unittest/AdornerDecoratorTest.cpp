/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerDecoratorTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "AdornerDecoratorTest.hpp"
#include "UnitTestSuites.hpp"
#include "../adorner/AdornerDecorator.hpp"
#include "../util/Entry.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AdornerDecoratorTest, Atoll::UnitTestSuites::Adorner());

AdornerDecoratorTest::AdornerDecoratorTest()
{
}
//------------------------------------------------------------------------------


AdornerDecoratorTest::~AdornerDecoratorTest()
{
}
//------------------------------------------------------------------------------

void AdornerDecoratorTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void AdornerDecoratorTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void AdornerDecoratorTest::testDecorator(void)
{
	UnicodeString str;
	//const UChar *sDebug;
	std::auto_ptr<AdornerDecorator> decorator(new AdornerDecorator());
	UnicodeString &decoratorString = decorator->GetDecoratorString();
	decorator->EnableDecoratorString(true);

	// Test
	//   Le 5<e>eme</e> élément
	str = "Le 5";
	decorator->AddContent(str);
	str = "<e>";
	decorator->AddNode(str);
	str = "eme";
	decorator->AddContent(str);
	str = "</e>";
	decorator->AddNode(str);
	str = " element";
	decorator->AddContent(str);
	// Decorate
	decorator->DecorateContent();
	//sDebug = decoratorString.getTerminatedBuffer(); // Debug string
	CPPUNIT_ASSERT(decoratorString == "<w pos=\"1\">Le</w> <w pos=\"2\">5</w><e><w pos=\"2\">eme</w></e> <w pos=\"3\">element</w>");
	decoratorString.remove();

	Entry e(1, 0, 100000);
	decorator->SetDecoratorPosition(e);
	str = "Note";
	decorator->AddContent(str);
	// Decorate
	decorator->DecorateContent();
	//sDebug = decoratorString.getTerminatedBuffer(); // Debug string
	CPPUNIT_ASSERT(decoratorString == "<w pos=\"100000\">Note</w>");
	decoratorString.remove();

	e.mUlEntPos = 1;
	decorator->SetDecoratorPosition(e);
	str = "fap.tat";
	decorator->AddContent(str);
	// Decorate
	decorator->DecorateContent();
	//sDebug = decoratorString.getTerminatedBuffer(); // Debug string
	if (U_ICU_VERSION_MAJOR_NUM > 3)
		CPPUNIT_ASSERT(decoratorString == "<w pos=\"1\">fap.tat</w>");
	else
		CPPUNIT_ASSERT(decoratorString == "<w pos=\"1\">fap</w>.<w pos=\"2\">tat</w>");
	decoratorString.remove();

	e.mUlEntPos = 1;
	decorator->SetDecoratorPosition(e);
	str = "fap?tat";
	decorator->AddContent(str);
	// Decorate
	decorator->DecorateContent();
	//sDebug = decoratorString.getTerminatedBuffer(); // Debug string
	CPPUNIT_ASSERT(decoratorString == "<w pos=\"1\">fap</w>?<w pos=\"2\">tat</w>");
	decoratorString.remove();
}
//------------------------------------------------------------------------------
