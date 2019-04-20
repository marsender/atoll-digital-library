/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerHandler.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerHandler.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "../indexer/Indexer.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <xercesc/sax2/Attributes.hpp>
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
XERCES_CPP_NAMESPACE_USE

IndexerHandler::IndexerHandler()
	:XercesBaseHandler()
{
	mIndexer = new Indexer();
}
//------------------------------------------------------------------------------

IndexerHandler::~IndexerHandler()
{
	delete mIndexer;
}
//------------------------------------------------------------------------------

void IndexerHandler::SetDbManager(DbManager *inDbMgr)
{
	mIndexer->SetDbManager(inDbMgr);
}
//------------------------------------------------------------------------------

void IndexerHandler::LoadIndexerConfig(const std::string &inFileName)
{
	mIndexer->LoadIndexerConfig(inFileName);
}
//------------------------------------------------------------------------------

void IndexerHandler::LoadRecordBreakerConfig(const std::string &inFileName)
{
	mIndexer->LoadRecordBreakerConfig(inFileName);
}
//------------------------------------------------------------------------------

void IndexerHandler::SetDocRef(unsigned int inNumDoc)
{
	mIndexer->SetIndexerDocRef(inNumDoc);
}
//------------------------------------------------------------------------------

void IndexerHandler::SetDstFileName(const std::string &inFileName)
{
	mIndexer->SetIndexerAdornerDstFileName(inFileName);
}
//------------------------------------------------------------------------------

void IndexerHandler::EnableRecordBreaker(bool inEnable)
{
	mIndexer->EnableIndexerOutputRecordBreaker(inEnable);
}
//------------------------------------------------------------------------------

void IndexerHandler::EnableDbDocStorage(bool inEnable)
{
	mIndexer->EnableIndexerOutputDbDocStorage(inEnable);
}
//------------------------------------------------------------------------------

bool IndexerHandler::InitHandler()
{
	bool isOk = true;
	
	if (mIndexer->GetNumDoc() == 0) {
		gLog.log(eTypLogError, "Err > IndexerHandler: Document number is not set");
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

void IndexerHandler::BeginDocument()
{
	// Call the parent
	XercesBaseHandler::BeginDocument();

	mIndexer->StartDocument();
}
//------------------------------------------------------------------------------

void IndexerHandler::EndDocument(bool inIsException)
{
	mIndexer->EndDocument(inIsException);

	// Add indexer errors
	AddNbError(mIndexer->GetNbError());

	// Call the parent
	XercesBaseHandler::EndDocument(inIsException);
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void IndexerHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const inLocalName,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update current element and attributes map
	XercesBaseHandler::startElement(inUri, inLocalName, inQName, inAttrs);

	// Send the start element notification to the indexer
	mIndexer->StartNode(mElem, mAttrMap);
}
//------------------------------------------------------------------------------

void IndexerHandler::endElement(const XMLCh * const inUri,
																const XMLCh * const inLocalName,
																const XMLCh * const inQName)
{
	// Send the end element notification to the indexer
	mIndexer->EndNode();

	XercesBaseHandler::endElement(inUri, inLocalName, inQName);
}
//------------------------------------------------------------------------------

void IndexerHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	// Send the characters notification to the indexer
	const UChar *str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	mIndexer->AddCharacters(str, (unsigned int)inLength);
}
//------------------------------------------------------------------------------

void IndexerHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	// Call the SAX character handler
	characters(inStr, inLength);
}
//------------------------------------------------------------------------------
