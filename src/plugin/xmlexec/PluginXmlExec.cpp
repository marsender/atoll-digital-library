/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXmlExec.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XmlExecParser.hpp"
#include "../../plugin/PluginKernel.hpp"
#include "../../plugin/PluginMessage.hpp"
#include "unicode/ustring.h"
#ifdef _WIN32
	#include "vld.h" // Visual Leak Detector (Memory leak detection)
#endif 
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! Atoll engine plugin that allows the execution of an xml query
class PluginXmlExec : public AtollPlugin::PluginServer::PluginXml
{
public:
	//! Constructor
	PluginXmlExec(Common::Logger &inLog);
	//! Destructor
	virtual ~PluginXmlExec();

	//! Check whether the plugin can run an xmlexec file
	bool CanRunXmlExecFile(const std::string &inFileName);
	//! Check whether the plugin can run an xmlexec buffer
	bool CanRunXmlExecBuffer();
	//! Check whether the plugin can run an xslt on a buffer
	bool CanRunXsltBuffer();

	//! Run the plugin with an xml file
	std::auto_ptr<AtollPlugin::PluginMessage> PluginRunXmlExecFile(const std::string &inFileName);
	//! Run the plugin with an xml buffer
	std::auto_ptr<AtollPlugin::PluginMessage> PluginRunXmlExecBuffer(const UChar *inStr, int32_t inLength);
	//! Run the xslt with an xml buffer and a stylesheet buffer
	std::auto_ptr<AtollPlugin::PluginMessage> PluginRunXsltBuffer(const UChar *inStr, int32_t inLength, const UChar *inXsl, int32_t inLengthXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
	//! Run the xslt with an xml buffer and a named precompiled stylesheet
	std::auto_ptr<AtollPlugin::PluginMessage> PluginRunXsltCompiled(const UChar *inStr, int32_t inLength, const UnicodeString &inXsltName, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
};
} // namespace Atoll
//------------------------------------------------------------------------------

using namespace std;
using namespace Common;
using namespace AtollPlugin;
using namespace AtollPluginXmlExec;
//------------------------------------------------------------------------------

//! Retrieve the engine version we're going to expect
extern "C" DEF_Export int GetEngineExpectedVersion()
{
	return DEF_PluginVersion;
}
//------------------------------------------------------------------------------

//! Tells us to register our functionality to a plugin kernel
extern "C" DEF_Export void RegisterPlugin(PluginKernel &inPluginKernel)
{
	PluginXmlExec *pluginXmlExec;
	std::auto_ptr<PluginServer::PluginXml> pluginXml;
	
	pluginXmlExec = new PluginXmlExec(inPluginKernel.GetLogger());
	pluginXml.reset(pluginXmlExec);

	inPluginKernel.GetPluginServer().AddPluginXml(pluginXml);
}
//------------------------------------------------------------------------------

PluginXmlExec::PluginXmlExec(Common::Logger &inLog)
	: PluginXml(inLog)
{
	mLog.log(eTypLogDebug, "Deb > Plugin Xml constructor");
}
//------------------------------------------------------------------------------

PluginXmlExec::~PluginXmlExec()
{
	mLog.log(eTypLogDebug, "Deb > Plugin Xml destructor");
}
//------------------------------------------------------------------------------

bool PluginXmlExec::CanRunXmlExecFile(const string &inFileName)
{
	return inFileName.find(".xml") == (inFileName.length() - 4);
}
//------------------------------------------------------------------------------

bool PluginXmlExec::CanRunXmlExecBuffer()
{
	return true;
}
//------------------------------------------------------------------------------

bool PluginXmlExec::CanRunXsltBuffer()
{
	return false;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXmlExec::PluginRunXmlExecFile(const string &inFileName)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	if (!CanRunXmlExecFile(inFileName)) {
		mLog.log(eTypLogError, "Err > Plugin cannot read xml file: %s", inFileName.c_str());
		return pluginMessage;
	}

	try {
		// Create the parser
		std::auto_ptr<XmlExecParser> xercesParser;
		const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
		xercesParser.reset(new XmlExecParser(SAX2XMLReader::Val_Auto, catalogFile,
			pluginMessage.get(), eTypHandlerXmlExecReq, mLog));
		// Parse
		pluginMessage->mIsOk = xercesParser->XercesParse(inFileName);
	}
	catch (const AppException &e) {
		mLog.log(eTypLogError, "Err > Plugin exception during parsing: %s", e.what());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during parsing: %s", e.what());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXmlExec::PluginRunXmlExecBuffer(const UChar *inStr, int32_t inLength)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	if (!CanRunXmlExecBuffer()) {
		mLog.log(eTypLogError, "Err > Plugin cannot read xml buffer");
		return pluginMessage;
	}

	try {
		// Create the parser
		std::auto_ptr<XmlExecParser> xercesParser;
		const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
		xercesParser.reset(new XmlExecParser(SAX2XMLReader::Val_Auto, catalogFile,
			pluginMessage.get(), eTypHandlerXmlExecReq, mLog));
		// Parse
		pluginMessage->mIsOk = xercesParser->XercesParse(inStr, inLength);
	}
	catch (const AppException &e) {
		mLog.log(eTypLogError, "Err > Plugin exception during parsing: %s", e.what());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during parsing: %s", e.what());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXmlExec::PluginRunXsltBuffer(const UChar *inStr, int32_t inLength,
	const UChar *inXsl, int32_t inLengthXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXmlExec::PluginRunXsltCompiled(const UChar *inStr, int32_t inLength,
	const UnicodeString &inXsltName, const Common::StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	return pluginMessage;
}
//------------------------------------------------------------------------------
