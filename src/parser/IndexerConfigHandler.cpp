/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigHandler.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerConfigHandler.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "../indexer/IndexerConfig.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <xercesc/sax2/Attributes.hpp>
//------------------------------------------------------------------------------

using namespace Atoll;
XERCES_CPP_NAMESPACE_USE

IndexerConfigHandler::IndexerConfigHandler()
	:XercesBaseHandler()
{
	mIndexerConfig = NULL;
}
//------------------------------------------------------------------------------

IndexerConfigHandler::~IndexerConfigHandler()
{
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::SetIndexerConfig(IndexerConfig *inIndexerConfig)
{
	mIndexerConfig = inIndexerConfig;
}
//------------------------------------------------------------------------------

bool IndexerConfigHandler::InitHandler()
{
	bool isOk = true;

	// The indexer config must have been set before parsing
	isOk &= (mIndexerConfig != NULL);

	return isOk;
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::BeginDocument()
{
	// Call the parent
	XercesBaseHandler::BeginDocument();

	mIndexerConfig->StartDocument();
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::EndDocument(bool inIsException)
{
	mIndexerConfig->EndDocument(inIsException);

	// Add indexer errors
	mNbError += mIndexerConfig->GetNbError();

	// Call the parent
	XercesBaseHandler::EndDocument(inIsException);
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void IndexerConfigHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const inLocalName,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update current element and attributes map
	XercesBaseHandler::startElement(inUri, inLocalName, inQName, inAttrs);

	// Send the start element notification to the IndexerConfig
	mIndexerConfig->StartNode(mElem, mAttrMap);
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::endElement(const XMLCh * const inUri,
																const XMLCh * const inLocalName,
																const XMLCh * const inQName)
{
	// Send the end element notification to the IndexerConfig
	mIndexerConfig->EndNode();

	XercesBaseHandler::endElement(inUri, inLocalName, inQName);
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	// Send the characters notification to the IndexerConfig
	const UChar *str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	mIndexerConfig->AddCharacters(str, (unsigned int)inLength);
}
//------------------------------------------------------------------------------

void IndexerConfigHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	// Call the SAX character handler
	characters(inStr, inLength);
}
//------------------------------------------------------------------------------
