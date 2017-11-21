/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXsltTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "PluginXsltTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineApi.hpp"
#include "../engine/EngineLib.hpp"
#include "../plugin/PluginMessage.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h"
#ifndef WIN32
	#include <unistd.h>
#endif
//------------------------------------------------------------------------------

#define DEF_XsltParamHighwords "atoll.highwords"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(PluginXsltTest, Atoll::UnitTestSuites::Core());

PluginXsltTest::PluginXsltTest()
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


PluginXsltTest::~PluginXsltTest()
{
}
//------------------------------------------------------------------------------

void PluginXsltTest::setUp(void)
{
	//gLog.log(eTypLogAction, "Act > UnitTest PluginXslt");
}
//------------------------------------------------------------------------------

void PluginXsltTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void PluginXsltTest::testXsltBuffer(void)
{
	UnicodeString uStr, uXsl;
	bool isOk, wantOutputFile = true;
	std::string testFileName = mPath + "xslt_test.html";
	std::string outFileName = mPath + "xslt_test_temp.html";

	std::string fullFileName = mPath + "xslt_test.xml";
	isOk = File2UnicodeString(uStr, fullFileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(fullFileName, isOk);

	std::string xsltFileName = mPath + "xslt_test.xsl";
	isOk = File2UnicodeString(uXsl, xsltFileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(xsltFileName, isOk);

	// Xslt plugin execution
	StringToUnicodeStringMap stylesheetParamMap;
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage;
	pluginMessage = XsltBuffer(uStr, uXsl, stylesheetParamMap);
	CPPUNIT_ASSERT_MESSAGE("XsltBuffer error", pluginMessage->mIsOk);

	if (wantOutputFile) {
		isOk = UnicodeString2File(pluginMessage->mMessage, outFileName.c_str());
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		// Text compare source and destination files
		isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);

		// Delete temporary file
		::remove(outFileName.c_str());
	}
}
//------------------------------------------------------------------------------

void PluginXsltTest::testXsltCompiled(void)
{
	// Run test on different stylesheets
	runXsltCompiled("identity");
	runXsltCompiled("highwords");

	// Enable if docbook-xsl is present
#ifndef _WIN32
	runXsltCompiled("docbook-xsl");
#endif

	// Message: Note: namesp. add : added namespace before processing
	//   Un exemple de livre DocBookSource tree node: #document. (file:///opt/dev/atoll/xsl/docbook-xsl-ns/common/utility.xsl, line 190, column 16.)
	// XSLT Message: Warn: no @xml:base: cannot add @xml:base to node-set root element
	//   Un exemple de livre DocBookSource tree node: book. (file:///opt/dev/atoll/xsl/docbook-xsl-ns/common/utility.xsl, line 190, column 16.)
	// XSLT Message: Warn: no @xml:base: relative paths may not work
	//   Un exemple de livre DocBookSource tree node: book. (file:///opt/dev/atoll/xsl/docbook-xsl-ns/common/utility.xsl, line 190, column 16.)
	//runXsltCompiled("docbook-xsl-ns");

	// Xslt compile stylesheet error: The function 'ends-with' was not found.
	// expression = 'not(ends-with(.,'.'))' Remaining tokens are:  ( 'ends-with' '(' '.' ',' ''.'' ')' ')')
	//runXsltCompiled("tei-xsl");
}
//------------------------------------------------------------------------------

void PluginXsltTest::runXsltCompiled(const char *inXsltName)
{
	UnicodeString uStr, uXsl;
	bool isOk, wantOutputFile = true;
	std::string testFileName, error;
	std::string outFileName = mPath + "xslt_test_temp.html";

	std::string fullFileName = mPath + "xslt_test.xml";
	isOk = File2UnicodeString(uStr, fullFileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(fullFileName, isOk);

	uXsl = inXsltName;
	testFileName = mPath;
	testFileName += "xslt_test_";
	testFileName += inXsltName;
	testFileName += ".html";

	// Set the stylesheet parameters with hilight positions
	// Example: "atoll.highwords", " 1 2 3 4 "
	StringToUnicodeStringMap stylesheetParamMap;
	UnicodeString paramValue(" 9 12 20 ");
	std::string paramKey(DEF_XsltParamHighwords);
	stylesheetParamMap[paramKey] = paramValue;

	// Xslt plugin execution
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage;
	pluginMessage = XsltCompiled(uStr, uXsl, stylesheetParamMap);
	error = "XsltCompiled error for ";
	error += inXsltName;
	CPPUNIT_ASSERT_MESSAGE(error, pluginMessage->mIsOk);

	if (wantOutputFile) {
		isOk = UnicodeString2File(pluginMessage->mMessage, outFileName.c_str());
		CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		// Cannot compare docbook result as the tabmat has some unique ids like <a name="N0x79f0d0N0x32bb390">
		if (strstr(inXsltName, "docbook-xsl") != inXsltName) {
			// Text compare source and destination files
			isOk = FileSystem::TextCompareFiles(testFileName, outFileName);
			CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);
		}
		// Delete temporary file
		::remove(outFileName.c_str());
	}
}
//------------------------------------------------------------------------------
