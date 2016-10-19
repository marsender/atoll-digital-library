/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXmlExecTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "PluginXmlExecTest.hpp"
#include "UnitTestSuites.hpp"
#include "../engine/EngineApi.hpp"
#include "../engine/EngineLib.hpp"
#include "../plugin/PluginMessage.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Register test suite
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(PluginXmlExecTest, Atoll::UnitTestSuites::Core());

PluginXmlExecTest::PluginXmlExecTest()
{
}
//------------------------------------------------------------------------------


PluginXmlExecTest::~PluginXmlExecTest()
{
}
//------------------------------------------------------------------------------

void PluginXmlExecTest::setUp(void)
{
	//gLog.log(eTypLogAction, "Act > UnitTest PluginXmlExec");
}
//------------------------------------------------------------------------------

void PluginXmlExecTest::tearDown(void)
{
}
//------------------------------------------------------------------------------

void PluginXmlExecTest::testXmlExecBuffer(void)
{
	bool isOk;

	const char *xmlReq = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><atoll-plugin-xmlexec version=\"1.0\"><ReqAll><ReqInfo><Version/></ReqInfo></ReqAll></atoll-plugin-xmlexec>";
	UnicodeString uStr(xmlReq);
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage;

	// XmlExec plugin execution
	pluginMessage = XmlExecBuffer(uStr);
	CPPUNIT_ASSERT_MESSAGE("XmlExecBuffer error", pluginMessage->mIsOk);
	std::string pluginBuffer;
	ConvertUnicodeString2String(pluginBuffer, pluginMessage->mMessage);

	// Get the version patern
	const char *version = Atoll::GetFullVersion();
	char *buf = new char[strlen(version) + 50];
	sprintf(buf, "<Version>%s</Version>", version);

	isOk = strstr(pluginBuffer.c_str(), buf);
	delete [] buf;
	CPPUNIT_ASSERT_MESSAGE("XmlExec content error", isOk);
}
//------------------------------------------------------------------------------

void PluginXmlExecTest::testAtollXmlExec(void)
{
	bool isOk;
	char *xmlRes;
	const char *xmlReq = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><atoll-plugin-xmlexec version=\"1.0\"><ReqAll><ReqInfo><Version/></ReqInfo></ReqAll></atoll-plugin-xmlexec>";

	// XmlExec plugin execution
	xmlRes = AtollXmlExec(xmlReq);

	// Get the version patern
	const char *version = Atoll::GetFullVersion();
	char *buf = new char[strlen(version) + 50];
	sprintf(buf, "<Version>%s</Version>", version);

	isOk = xmlRes && (strstr(xmlRes, buf) != NULL);
	delete [] buf;

	// Free the memory allocated by AtollXmlExec
	AtollXmlFree(xmlRes);

	CPPUNIT_ASSERT_MESSAGE("XmlExec error", isOk);
}
//------------------------------------------------------------------------------
