/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnicodeUtilTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "UnicodeUtilTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/StdString.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h" // u_strstr, u_strchr...
#ifndef WIN32
	#include <unistd.h>
#endif
//------------------------------------------------------------------------------

static bool gInitString = false;
U_STRING_DECL(cStrTestStringDecl, "test", 4);

using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UnicodeUtilTest, Atoll::UnitTestSuites::Common());

UnicodeUtilTest::UnicodeUtilTest()
{
	if (!gInitString) {
		U_STRING_INIT(cStrTestStringDecl, "test", 4);
		gInitString = true;
	}
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------


UnicodeUtilTest::~UnicodeUtilTest()
{
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::setUp()
{
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::tearDown()
{
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testStringDecl()
{
	UnicodeString test("test");
	CPPUNIT_ASSERT_MESSAGE("String declaration", test == cStrTestStringDecl);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testFile2UnicodeString()
{
	// Run test on different xml files
	// The files must be Utf-8 encoded !
	runFile2UnicodeString("utf8_test.xml");
	runFile2UnicodeString("utf8_demo.xml");
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::runFile2UnicodeString(const char *inFile)
{
	bool isOk;
	UnicodeString uStr;
	std::string mess, fileName, outFileName, fileExt;

	fileName = mPath + inFile;
	fileExt = FileSystem::GetExtension(fileName);
	outFileName = mPath + FileSystem::GetFileNameWithoutExt(fileName) + "_temp." + fileExt;

	mess = inFile;
	mess += " File extension must be xml";
	CPPUNIT_ASSERT_MESSAGE(mess.c_str(), fileExt == "xml");

	// Load the file into the unicode string
	// File must absolutely be encoded in utf-8
	// and have same CrLf as the plateform
	isOk = File2UnicodeString(uStr, fileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(inFile, isOk);

	// First char must be '<' for an xml file
	const UChar *sBuffer = uStr.getTerminatedBuffer();
	if (*sBuffer != '<') {
		mess = inFile;
		if (*sBuffer == 65279) {
			mess += " Xml file begining with Utf-8 BOM";
			CPPUNIT_ASSERT_MESSAGE(mess.c_str(), false);
		}
		else if (*sBuffer == 65533) {
			mess += " Xml file begining with Ucs-2 Little Endian BOM";
			CPPUNIT_ASSERT_MESSAGE(mess.c_str(), false);
		}
		else {
			mess += " Xml file begining with char ";
			StdString::AppendInt(mess, *sBuffer);
			CPPUNIT_ASSERT_MESSAGE(mess.c_str(), false);
		}
	}

	// Save the unicode string into the file
	isOk = UnicodeString2File(uStr, outFileName.c_str());
	CPPUNIT_ASSERT_MESSAGE(inFile, isOk);

	// Binary compare source and destination files
	isOk = FileSystem::BinaryCompareFiles(fileName, outFileName);
	CPPUNIT_ASSERT_MESSAGE(outFileName, isOk);

	// Delete temporary file
	::remove(outFileName.c_str());
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeString2String()
{
	std::string str, str1;
	UnicodeString uStr, uStr1;

	// Test empty unicode string conversion
	str = UnicodeString2String(uStr);
	CPPUNIT_ASSERT(str == "");

	// Test unicode string to string conversion
	uStr = "test";
	str = UnicodeString2String(uStr);
	CPPUNIT_ASSERT(str == "test");

	// Test unicode buffer to string conversion
	UChar2String(str, uStr.getBuffer(), uStr.length());
	CPPUNIT_ASSERT(str == "test");
	UChar2String(str, uStr.getBuffer(), 2);
	CPPUNIT_ASSERT(str == "te");

	// Test standard buffer to unicode string conversion
	Char2UnicodeString(uStr, "12345");
	CPPUNIT_ASSERT(uStr == "12345");
	Char2UnicodeString(uStr, "12345", 3); // With buffer length
	CPPUNIT_ASSERT(uStr == "123");
	Char2UnicodeString(uStr, "12345", 0); // With buffer length
	CPPUNIT_ASSERT(uStr == "");

	// Test conversion deconversion
	const UChar cStr[] = { 0x00B7, '-', 0x00E9, '-', 0x00B7, 0 }; // "·-é-·";
	uStr = cStr;
	str = UnicodeString2String(uStr);
	CPPUNIT_ASSERT(str == "·-é-·");
	UChar2String(str1, uStr.getBuffer(), uStr.length());
	CPPUNIT_ASSERT(str == str1);
	Char2UnicodeString(uStr1, str.c_str());
	CPPUNIT_ASSERT(uStr == uStr1);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringStrip()
{
	UnicodeString str, res;
	UChar begin[2], end[2];

	*begin = '<'; *(begin + 1) = 0;
	*end = '>'; *(end + 1) = 0;

	str = "<>";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "");

	str = "<wd>a</wd>";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "a");

	str = "a<wd>b</wd>";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "ab");

	str = "a<wd>b</wd>c";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "abc");

	str = "<";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "");

	str = "<a";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "");

	str = "a<b";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "a");

	str = "a<";
	UnicodeStringStrip(res, str, begin, end);
	CPPUNIT_ASSERT(res == "a");

	str = "<a";
	UnicodeStringStrip(res, str, begin, NULL);
	CPPUNIT_ASSERT(res == "a");

	str = "a<b";
	UnicodeStringStrip(res, str, begin, NULL);
	CPPUNIT_ASSERT(res == "ab");

	str = "a<";
	UnicodeStringStrip(res, str, begin, NULL);
	CPPUNIT_ASSERT(res == "a");
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringGetContent()
{
	UnicodeString str, res;
	UChar begin[2], end[2];

	*begin = '<'; *(begin + 1) = 0;
	*end = '>'; *(end + 1) = 0;

	str = "<>";
	UnicodeStringGetContent(res, str, begin, end);
	CPPUNIT_ASSERT(res == "");

	str = "<a>";
	UnicodeStringGetContent(res, str, begin, end);
	CPPUNIT_ASSERT(res == "a");

	str = "<a><b>";
	UnicodeStringGetContent(res, str, begin, end, 1);
	CPPUNIT_ASSERT(res == "b");
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringGetLine()
{
	bool isOk;
	UnicodeString res, str, line1, line2;

	line1 = "1";
	line2 = "2";
	str += line1;
	str += "\n";
	str += line2;

	isOk = UnicodeStringGetLine(res, str, 0);
	CPPUNIT_ASSERT(isOk && res == line1);

	isOk = UnicodeStringGetLine(res, str, 1);
	CPPUNIT_ASSERT(isOk && res == line2);

	isOk = UnicodeStringGetLine(res, str, 2);
	CPPUNIT_ASSERT(isOk == false);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringClearCrLf()
{
	// Creates two strings: input and output
	const UChar cTestStr[] = {
		' ', '\t', '\r', '\n',
		'o', 'n', 'e',
		' ', '\t', '\r', '\n',
		't', 'w', 'o',
		' ', '\t', '\r', '\n',
		0 };
	const UChar cResStr[] = { 'o', 'n', 'e', ' ', 't', 'w', 'o', 0 };

	// Check if the two strings are equal after clearing the line feeds
	UnicodeString str = cTestStr;
	UnicodeStringClearCrLf(str);
  CPPUNIT_ASSERT(str == cResStr);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringHasSpace()
{
	// Creates two strings: with and without spaces
	const UChar cTestStrSpace[] = { 't', 'e', ' ', 's', 't', 0 };
	const UChar cTestStrNoSpace[] = { 't', 'e', 's', 't', 0 };

	UnicodeString str;
	
	// Check the two strings
	str = cTestStrSpace;
  CPPUNIT_ASSERT(UnicodeStringHasSpace(str) == true);
	str = cTestStrNoSpace;
  CPPUNIT_ASSERT(UnicodeStringHasSpace(str) == false);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringConversion()
{
	std::string str, str2;
	UnicodeString uStr, uStr2;
	std::string fileName = mPath + "utf8_demo.xml";
	const UChar cStr[] = { 0x00B7, '-', 0x00E9, '-', 0x00B7, 0 }; // "·-é-·";

	// Test unicode string conversion with accents
	uStr = cStr;
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr));
	CPPUNIT_ASSERT(str == "·-é-·");
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str));
	CPPUNIT_ASSERT(uStr == uStr2);
	str = "·-é-·";
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr, str));
	CPPUNIT_ASSERT(uStr == cStr);
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str2, uStr));
	CPPUNIT_ASSERT(str == str2);

	// Test unicode string "SCSU" conversion with accents
	uStr = cStr;
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr, "SCSU"));
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str, "SCSU"));
	CPPUNIT_ASSERT(uStr == uStr2);
	str = "·-é-·";
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr, str, "SCSU"));
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str2, uStr, "SCSU"));
	CPPUNIT_ASSERT(str == str2);

	// Test unicode string "utf-8" conversion with accents
	uStr = cStr;
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr, "utf-8"));
	CPPUNIT_ASSERT(str == "·-é-·");
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str, "utf-8"));
	CPPUNIT_ASSERT(uStr == uStr2);
	str = "·-é-·";
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr, str, "utf-8"));
	CPPUNIT_ASSERT(uStr == cStr);
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str2, uStr, "utf-8"));
	CPPUNIT_ASSERT(str == str2);

	// Test conversions from file content
	CPPUNIT_ASSERT(File2UnicodeString(uStr, fileName.c_str()));
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr));
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str));
	CPPUNIT_ASSERT_MESSAGE("UnicodeString: Bad conversion", uStr2 == uStr);
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr, "SCSU"));
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str, "SCSU"));
	CPPUNIT_ASSERT_MESSAGE("UnicodeString: Bad SCSU conversion", uStr2 == uStr);
	CPPUNIT_ASSERT(ConvertUnicodeString2String(str, uStr, "utf-8"));
	CPPUNIT_ASSERT(ConvertString2UnicodeString(uStr2, str, "utf-8"));
	CPPUNIT_ASSERT_MESSAGE("UnicodeString: Bad utf-8 conversion", uStr2 == uStr);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringToLower()
{
	UnicodeString uStr, uStr2;

	uStr = "TEST";
	uStr.toLower();
	CPPUNIT_ASSERT(uStr == "test");

	// Test with accents
	UChar s1[] = { 'b', 0x00C0, 0x00C9, 'i', 0 }; // "bÀÉi"
	UChar s2[] = { 'b', 0x00E0, 0x00E9, 'i', 0 }; // "bàéi"
	uStr = s1;
	uStr.toLower();
	CPPUNIT_ASSERT(uStr == s2);
}
//------------------------------------------------------------------------------

void UnicodeUtilTest::testUnicodeStringAddXmlEntities()
{
	UnicodeString uStr, uStr2;

	uStr2 = "test";
	UnicodeStringAddXmlEntities(uStr, uStr2);
	CPPUNIT_ASSERT(uStr == "test");

	uStr2 = " & < > \" ' ";
	UnicodeStringAddXmlEntities(uStr, uStr2);
	CPPUNIT_ASSERT(uStr == "test &amp; &lt; &gt; &quot; &apos; ");

	uStr = "";
	uStr2 = "test & next";
	UnicodeStringAddXmlEntities(uStr, uStr2.getBuffer(), 6);
	CPPUNIT_ASSERT(uStr == "test &amp;");
}
//------------------------------------------------------------------------------
