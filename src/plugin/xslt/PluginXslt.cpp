/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginXslt.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XsltManager.hpp"
#include "XsltTransformer.hpp"
#include "../../common/XercesParser.hpp"
#include "../../plugin/PluginKernel.hpp"
#include "../../plugin/PluginMessage.hpp"
#include "unicode/ustring.h"
#ifdef _WIN32
	#include "vld.h" // Visual Leak Detector (Memory leak detection)
#endif 
//------------------------------------------------------------------------------

namespace AtollPluginXslt
{

//! Atoll engine plugin that allows the execution of xsl transformations
class PluginXslt : public AtollPlugin::PluginServer::PluginXml
{
private:
	XsltManager *mXsltManager; //!< Xslt manager

public:
	//! Constructor
	PluginXslt(Common::Logger &inLog);
	//! Destructor
	virtual ~PluginXslt();

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
using namespace AtollPluginXslt;
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
	PluginXslt *pluginXslt;
	std::auto_ptr<PluginServer::PluginXml> pluginXml;
	
	pluginXslt = new PluginXslt(inPluginKernel.GetLogger());
	pluginXml.reset(pluginXslt);

	inPluginKernel.GetPluginServer().AddPluginXml(pluginXml);
}
//------------------------------------------------------------------------------

PluginXslt::PluginXslt(Common::Logger &inLog)
	: PluginXml(inLog)
{
	mLog.log(eTypLogDebug, "Deb > Plugin Xslt constructor");

	mXsltManager = new XsltManager(inLog);
}
//------------------------------------------------------------------------------

PluginXslt::~PluginXslt()
{
	mLog.log(eTypLogDebug, "Deb > Plugin Xslt destructor");

	delete mXsltManager;
}
//------------------------------------------------------------------------------

bool PluginXslt::CanRunXmlExecFile(const string &inFileName)
{
	return false;
}
//------------------------------------------------------------------------------

bool PluginXslt::CanRunXmlExecBuffer()
{
	return false;
}
//------------------------------------------------------------------------------

bool PluginXslt::CanRunXsltBuffer()
{
	return true;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXslt::PluginRunXmlExecFile(const string &inFileName)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXslt::PluginRunXmlExecBuffer(const UChar *inStr, int32_t inLength)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXslt::PluginRunXsltBuffer(const UChar *inStr, int32_t inLength,
	const UChar *inXsl, int32_t inLengthXsl, const StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	if (!CanRunXsltBuffer()) {
		mLog.log(eTypLogError, "Err > Plugin cannot read xml buffer");
		return pluginMessage;
	}

	try {
		// Create the xlst
		std::auto_ptr<XsltTransformer> xsltTransformer;
		const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
		xsltTransformer.reset(new XsltTransformer(SAX2XMLReader::Val_Auto, catalogFile, pluginMessage.get(), mLog, mXsltManager));
		pluginMessage->mIsOk = xsltTransformer->XslTransformWithBuffer(inStr, inLength, inXsl, inLengthXsl, inStylesheetParamMap);
	}
	catch (const AppException &e) {
		mLog.log(eTypLogError, "Err > Plugin exception during xslt: %s", e.what());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xslt: %s", e.what());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::auto_ptr<PluginMessage> PluginXslt::PluginRunXsltCompiled(const UChar *inStr, int32_t inLength,
	const UnicodeString &inXsltName, const StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::auto_ptr<PluginMessage> pluginMessage(new PluginMessage());
	pluginMessage->mIsOk = false;

	if (!CanRunXsltBuffer()) {
		mLog.log(eTypLogError, "Err > Plugin cannot read xml buffer");
		return pluginMessage;
	}

	try {
		// Create the xlst
		std::auto_ptr<XsltTransformer> xsltTransformer;
		const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
		xsltTransformer.reset(new XsltTransformer(SAX2XMLReader::Val_Auto, catalogFile, pluginMessage.get(), mLog, mXsltManager));
		pluginMessage->mIsOk = xsltTransformer->XslTransformWithCompiled(inStr, inLength, inXsltName, inStylesheetParamMap);
	}
	catch (const AppException &e) {
		mLog.log(eTypLogError, "Err > Plugin exception during xslt: %s", e.what());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xslt: %s", e.what());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------
