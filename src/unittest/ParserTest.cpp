/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ParserTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ParserTest.hpp"
#include "UnitTestSuites.hpp"
//#include "../common/XercesBaseHandler.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "../parser/Parser.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ParserTest, Atoll::UnitTestSuites::Parser());

ParserTest::ParserTest() :
	mParser(0)
{
	mPath = "./data_unittest/";
}
//------------------------------------------------------------------------------

ParserTest::~ParserTest()
{
}
//------------------------------------------------------------------------------

void ParserTest::setUp(void)
{
	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	mParser = new Parser(SAX2XMLReader::Val_Never, catalogFile, eTypHandlerBase);
}
//------------------------------------------------------------------------------

void ParserTest::tearDown(void)
{
	delete mParser;
}
//------------------------------------------------------------------------------

void ParserTest::testXercesStringLocalForm(void)
{
	std::string stdString;

	XMLCh xercesBuf[] = { 't', 'e', 's', 't', 0 };
	XercesString xercesString1(xercesBuf);
	stdString = xercesString1.localForm();
  CPPUNIT_ASSERT(stdString == "test");

	XMLCh *xmlStr = XMLString::transcode("test");
	XercesString xercesString2(xmlStr);
	stdString = xercesString2.localForm();
	XMLString::release(&xmlStr);
  CPPUNIT_ASSERT(stdString == "test");
}
//------------------------------------------------------------------------------

void ParserTest::testXercesStringUChar(void)
{
	XMLCh xercesBuf[] = { 't', (XMLCh)'é', 's', 't', 0 };
	UChar uBuf[] = { 't', (UChar)'é', 's', 't', 0 };

	XercesString *cnv = new XercesString();
	const UChar *str = cnv->ConvertToUChar(xercesBuf);
	int32_t comp = u_memcmp(uBuf, str, u_strlen(uBuf));
	delete cnv;

  CPPUNIT_ASSERT(comp == 0);
}
//------------------------------------------------------------------------------

void ParserTest::testXercesParseFile(void)
{
	bool isOk;

	std::string fullFileName = mPath + "utf8_test.xml";
	isOk = mParser->XercesParse(fullFileName);
  CPPUNIT_ASSERT(isOk);
}
//------------------------------------------------------------------------------

void ParserTest::testXercesParseString(void)
{
	bool isOk;

	UnicodeString str;
	std::string fullFileName = mPath + "utf8_test.xml";
	File2UnicodeString(str, fullFileName.c_str());
	isOk = mParser->XercesParse(str.getBuffer(), str.length());
  CPPUNIT_ASSERT(isOk);
}
//------------------------------------------------------------------------------
