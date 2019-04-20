/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfigHandler.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "RecordBreakerConfigHandler.hpp"
#include "../recordbreaker/RecordBreakerConfig.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <xercesc/sax2/Attributes.hpp>
//------------------------------------------------------------------------------

using namespace Atoll;
XERCES_CPP_NAMESPACE_USE

RecordBreakerConfigHandler::RecordBreakerConfigHandler()
	:XercesBaseHandler()
{
	mRecordBreakerConfig = NULL;
}
//------------------------------------------------------------------------------

RecordBreakerConfigHandler::~RecordBreakerConfigHandler()
{
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::SetRecordBreakerConfig(RecordBreakerConfig *inRecordBreakerConfig)
{
	mRecordBreakerConfig = inRecordBreakerConfig;
}
//------------------------------------------------------------------------------

bool RecordBreakerConfigHandler::InitHandler()
{
	bool isOk = true;

	// The record breaker config must have been set before parsing
	isOk &= (mRecordBreakerConfig != NULL);

	return isOk;
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::BeginDocument()
{
	// Call the parent
	XercesBaseHandler::BeginDocument();

	mRecordBreakerConfig->StartDocument();
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::EndDocument(bool inIsException)
{
	mRecordBreakerConfig->EndDocument(inIsException);

	// Add record breaker errors
	AddNbError(mRecordBreakerConfig->GetNbError());

	// Call the parent
	XercesBaseHandler::EndDocument(inIsException);
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void RecordBreakerConfigHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const inLocalName,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update current element and attributes map
	XercesBaseHandler::startElement(inUri, inLocalName, inQName, inAttrs);

	// Send the start element notification to the RecordBreakerConfig
	mRecordBreakerConfig->StartNode(mElem, mAttrMap);
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::endElement(const XMLCh * const inUri,
																const XMLCh * const inLocalName,
																const XMLCh * const inQName)
{
	// Send the end element notification to the RecordBreakerConfig
	mRecordBreakerConfig->EndNode();

	XercesBaseHandler::endElement(inUri, inLocalName, inQName);
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	// Send the characters notification to the RecordBreakerConfig
	const UChar *str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	mRecordBreakerConfig->AddCharacters(str, (unsigned int)inLength);
}
//------------------------------------------------------------------------------

void RecordBreakerConfigHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	// Call the SAX character handler
	characters(inStr, inLength);
}
//------------------------------------------------------------------------------
