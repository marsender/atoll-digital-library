/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "XercesTest.hpp"
#include "UnitTestSuites.hpp"
#include "../common/XercesCatalogResolver.hpp"
#include "../common/XercesString.hpp"
#include "../common/XercesParser.hpp"
//------------------------------------------------------------------------------

using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(XercesTest, Atoll::UnitTestSuites::Parser());

XercesTest::XercesTest()
{
}
//------------------------------------------------------------------------------


XercesTest::~XercesTest()
{
}
//------------------------------------------------------------------------------

void XercesTest::setUp()
{
}
//------------------------------------------------------------------------------

void XercesTest::tearDown()
{
}
//------------------------------------------------------------------------------

void XercesTest::testXercesString()
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

void XercesTest::testXercesCatalogResolver()
{
	bool isOk;
	std::string uri, publicId;
	XercesCatalogResolver catalog;

	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	isOk = catalog.LoadCatalog(catalogFile);
  CPPUNIT_ASSERT_MESSAGE(catalogFile.c_str(), isOk);

	publicId = "-//OASIS//DTD DocBook XML V5.0//EN";
	uri = catalog.LookupPublicId(publicId);
#ifdef WIN32
	isOk = (uri == "file:///C:/Program%20Files/XML%20Copy%20Editor/dtd/docbook/5.0/docbook.dtd")
		|| (uri == "file:///C:/Program%20Files%20(x86)/XML%20Copy%20Editor/dtd/docbook/5.0/docbook.dtd");
#else
  isOk = (uri == "file:///opt/dev/tools/xmlcopyeditor/dtd/docbook/5.0/docbook.dtd");
#endif
  CPPUNIT_ASSERT_MESSAGE(uri.c_str(), isOk);
}
//------------------------------------------------------------------------------
