/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "AdornerTest.hpp"
#include "UnitTestSuites.hpp"
#include "../adorner/Adorner.hpp"
#ifndef WIN32
	#include <unistd.h>
#endif
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AdornerTest, Atoll::UnitTestSuites::Adorner());

AdornerTest::AdornerTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


AdornerTest::~AdornerTest()
{
}
//------------------------------------------------------------------------------

void AdornerTest::setUp(void)
{
}
//------------------------------------------------------------------------------

void AdornerTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void AdornerTest::testAdornContent(void)
{
	//const UChar *sDebug;
	bool wantOutputFile = true;
	unsigned long nbError;
	std::string elem;
	std::string attrName;
	UnicodeString attrValue;
	StringToUnicodeStringMap attrMap;
	UnicodeString text;
	unsigned int numDoc = 1;
	std::string testFileName = mPath + "test_adorner.xml";
	std::string outFileName = mPath + "test_adorner_temp.xml";

	std::auto_ptr<Adorner> adorner(new Adorner());
	adorner->EnableAdornerString(true);
	adorner->SetAdornerDocRef(numDoc);
	if (wantOutputFile)
		adorner->SetAdornerDstFileName(outFileName);
	adorner->StartDocument();

	UnicodeString &adornerString = adorner->GetAdornerString();
	adornerString.remove(); // Remove the xml file header

	// Opens a node
	elem = "adorner_test";
	attrName = "id";
	attrValue = "123";
	attrMap[attrName] = attrValue;
	adorner->StartNode(elem, attrMap);
	attrMap.clear();
	// Appends text
	text = "Full text content";
	adorner->AddCharacters(text.getTerminatedBuffer(), text.length());
	// Close the node
	adorner->EndNode();
	// Check the adorner content
	adorner->ForceDecorate();
	//sDebug = adornerString.getTerminatedBuffer(); // Debug string
	CPPUNIT_ASSERT(adornerString == "<adorner_test id=\"123\"><w pos=\"1\">Full</w> <w pos=\"2\">text</w> <w pos=\"3\">content</w></adorner_test>");
	adornerString.remove();

	// Test
	//   Le 5<e>eme</e> élément
	text = "\nLe 5";
	adorner->AddCharacters(text.getTerminatedBuffer(), text.length());
	elem = "e";
	adorner->StartNode(elem, attrMap);
	text = "eme";
	adorner->AddCharacters(text.getTerminatedBuffer(), text.length());
	adorner->EndNode();
	text = " element";
	adorner->AddCharacters(text.getTerminatedBuffer(), text.length());
	// Check the adorner content
	adorner->ForceDecorate();
	//sDebug = adornerString.getTerminatedBuffer(); // Debug string
	CPPUNIT_ASSERT(adornerString == "\n<w pos=\"4\">Le</w> <w pos=\"5\">5</w><e><w pos=\"5\">eme</w></e> <w pos=\"6\">element</w>");
	adornerString.remove();

	// Close the document
	adorner->EndDocument(false);
	adornerString.remove();
	nbError = adorner->GetNbError();

	CPPUNIT_ASSERT(nbError == 0);

	if (wantOutputFile) {
		// Text compare source and destination files
		bool isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		// Delete temporary file
		::remove(outFileName.c_str());
	}
}
//------------------------------------------------------------------------------
