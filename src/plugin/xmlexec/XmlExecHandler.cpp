/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecHandler.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XmlExecHandler.hpp"
#include "XmlExecRunner.hpp"
#include "../../common/UnicodeUtil.hpp"
#include "../../common/XercesString.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <xercesc/sax2/Attributes.hpp>
//------------------------------------------------------------------------------

using namespace Common;
using namespace AtollPluginXmlExec;
XERCES_CPP_NAMESPACE_USE

XmlExecHandler::XmlExecHandler(AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog)
	:XercesBaseHandler(),
	mLog(inLog)
{
	mRunner = new XmlExecRunner(inPluginMessage, mLog);
}
//------------------------------------------------------------------------------

XmlExecHandler::~XmlExecHandler()
{
	delete mRunner;
}
//------------------------------------------------------------------------------

bool XmlExecHandler::InitHandler()
{
	bool isOk = true;

	//isOk &= mRunner->RunnerInitDocument(inFileName);

	return isOk;
}
//------------------------------------------------------------------------------

void XmlExecHandler::BeginDocument()
{
	// Call the parent
	XercesBaseHandler::BeginDocument();

	mRunner->StartDocument();
}
//------------------------------------------------------------------------------

void XmlExecHandler::EndDocument(bool inIsException)
{
	mRunner->EndDocument(inIsException);

	// Add runner errors
	mNbError += mRunner->GetNbError();

	// Call the parent
	XercesBaseHandler::EndDocument(inIsException);
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void XmlExecHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const inLocalName,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update current element and attributes map
	XercesBaseHandler::startElement(inUri, inLocalName, inQName, inAttrs);

	// Send the start element notification to the runner
	mRunner->StartNode(mElem, mAttrMap);
}
//------------------------------------------------------------------------------

void XmlExecHandler::endElement(const XMLCh * const inUri,
																const XMLCh * const inLocalName,
																const XMLCh * const inQName)
{
	// Send the end element notification to the runner
	mRunner->EndNode();

	XercesBaseHandler::endElement(inUri, inLocalName, inQName);
}
//------------------------------------------------------------------------------

void XmlExecHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	const UChar *str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	mRunner->AddCharacters(str, (unsigned int)inLength);
}
//------------------------------------------------------------------------------

void XmlExecHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	// Call the SAX character handler
	characters(inStr, inLength);

	/*
	const UChar *pos = str;
	while ((pos = u_strchr(pos, 0x000A)) != NULL) {
		pos++;
		mCurLine++;
	}
	*/
}
//------------------------------------------------------------------------------
