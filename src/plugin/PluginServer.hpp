/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginServer.hpp

*******************************************************************************/

#ifndef __PluginServer_HPP
#define __PluginServer_HPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include "../common/Containers.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <vector>
#include <string>
#include <memory> // for std::unique_ptr
//------------------------------------------------------------------------------

namespace Common
{
	class Logger;
}

namespace AtollPlugin
{

class PluginMessage;

//! Server that allows plugins to register themselves and applications to search for a registred plugin
class PluginServer
{
public:
	//! Plugin xml
	class PluginXml
	{
	protected:
		Common::Logger &mLog;

	public:
		PluginXml(Common::Logger &inLog)
			: mLog(inLog)
		{}
		//! Destructor
		virtual ~PluginXml() {}

		//! Check whether the plugin can run an xmlexec file
		virtual bool CanRunXmlExecFile(const std::string &inFileName) = 0;
		//! Check whether the plugin can run an xmlexec buffer
		virtual bool CanRunXmlExecBuffer() = 0;
		//! Check whether the plugin can run an xslt on a buffer
		virtual bool CanRunXsltBuffer() = 0;

		//! Run the plugin with an xml file
		virtual std::unique_ptr<PluginMessage> PluginRunXmlExecFile(const std::string &inFileName) = 0;
		//! Run the plugin with an xml buffer
		virtual std::unique_ptr<PluginMessage> PluginRunXmlExecBuffer(const UChar *inStr, int32_t inLength) = 0;
		//! Run the xslt with an xml buffer and a stylesheet buffer
		virtual std::unique_ptr<PluginMessage> PluginRunXsltBuffer(const UChar *inStr, int32_t inLength, const UChar *inXsl, int32_t inLengthXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap) = 0;
		//! Run the xslt with an xml buffer and a named precompiled stylesheet
		virtual std::unique_ptr<AtollPlugin::PluginMessage> PluginRunXsltCompiled(const UChar *inStr, int32_t inLength, const UnicodeString &inXsltName, const Common::StringToUnicodeStringMap &inStylesheetParamMap) = 0;
	};

	DEF_Export PluginServer(Common::Logger &inLog);
	DEF_Export ~PluginServer();

	//! Allows plugins to add themselves
	DEF_Export void AddPluginXml(PluginXml *inPlugin);
	//! Delete all registred plugins
	DEF_Export void DeletePlugins();

	//! Run xml by searching for a matching xml reader
	/*
	DEF_Export std::unique_ptr<PluginMessage> RunXmlFile(const std::string &inFileName) {
		for (PluginXmlVector::iterator It = mPluginXmlVector.begin();
		     It != mPluginXmlVector.end(); ++It) {
			if ((*It)->CanRunXmlExecFile(inFileName))
				return (*It)->PluginRunXmlExecFile(inFileName);
		}
		throw Common::AppException(Common::AppException::InternalError, "Invalid or unsupported xml file");
	}
	*/

	//! Search for an xmlexec file reader plugin
	DEF_Export PluginXml *GetPluginXmlExecFile(const std::string &inFileName);
	//! Search for an xmlexec buffer reader plugin
	DEF_Export PluginXml *GetPluginXmlExecBuffer();
	//! Search for an xslt buffer plugin
	DEF_Export PluginXml *GetPluginXsltBuffer();

private:
	//! List of xml plugins
	typedef std::vector<PluginXml *> PluginXmlVector;

	Common::Logger &mLog;
	PluginXmlVector mPluginXmlVector; //!< All available xml plugins
};

} // namespace AtollPlugin

//------------------------------------------------------------------------------
#endif
