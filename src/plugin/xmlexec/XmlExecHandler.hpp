/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecHandler.hpp

*******************************************************************************/

#ifndef __XmlExecHandler_HPP
#define __XmlExecHandler_HPP
//------------------------------------------------------------------------------

#include "../../common/XercesBaseHandler.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace AtollPlugin
{
class PluginMessage;
}

namespace AtollPluginXmlExec
{
class XmlExecRunner;

//! XmlExec engine handler
/**
	Logic:
		- Receive notification of parser events and send it to the xmlexec runner
		- Xerces strings are converted in unicode strings
	Xerces API:
		http://xerces.apache.org/xerces-c/apiDocs-3/classDefaultHandler.html
*/
class XmlExecHandler : public Common::XercesBaseHandler
{
private:
	XmlExecRunner *mRunner;
	Common::Logger &mLog;

public:
	XmlExecHandler(AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog);
	virtual ~XmlExecHandler();

	//! Init the handler before parsing
	bool InitHandler();

	//! Handler called before parsing
  virtual void BeginDocument();
	//! Handler called after parsing
  virtual void EndDocument(bool inIsException);

	// -----------------------------------------------------------------------
	//  Handlers for the SAX DocumentHandler interface
	// -----------------------------------------------------------------------
	virtual void startElement(const XMLCh * const uri, const XMLCh * const localname,
										const XMLCh * const qname, const Attributes &attrs);
	virtual void endElement(const XMLCh * const uri, const XMLCh * const localname,
										const XMLCh * const qname);
	virtual void characters(const XMLCh * const chars, const XmlLength length);
	virtual void ignorableWhitespace(const XMLCh * const chars, const XmlLength length);
};

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
