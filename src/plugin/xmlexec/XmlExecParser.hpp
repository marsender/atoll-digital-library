/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecParser.hpp

*******************************************************************************/

#ifndef __XmlExecParser_HPP
#define __XmlExecParser_HPP
//------------------------------------------------------------------------------

#include "../../common/XercesParser.hpp"
#include "unicode/ustring.h"
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace AtollPlugin
{
class PluginMessage;
}

namespace AtollPluginXmlExec
{

//! Enum of handlers for the XmlExec parser
enum eTypHandlerXmlExec {
	eTypHandlerXmlExecNone,
	eTypHandlerXmlExecBase,
	eTypHandlerXmlExecReq,
	eTypHandlerXmlExecEnd
};

//! XmlExec engine parser
class XmlExecParser : public Common::XercesParser
{
private:
	eTypHandlerXmlExec mTypHandler;
	Common::Logger &mLog;

	bool XercesInitHandler();

public:
	XmlExecParser(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile,
		AtollPlugin::PluginMessage *inPluginMessage, eTypHandlerXmlExec inTypHandler, Common::Logger &inLog);
	virtual ~XmlExecParser();

	//! Parse an xml file
	virtual bool XercesParse(const std::string &inFileName);
	//! Parse an xml buffer
	virtual bool XercesParse(const UChar *inStr, int32_t inLength = -1);
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
