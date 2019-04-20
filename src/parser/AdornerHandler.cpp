/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerHandler.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "AdornerHandler.hpp"
#include "../adorner/Adorner.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <xercesc/sax2/Attributes.hpp>
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
XERCES_CPP_NAMESPACE_USE

AdornerHandler::AdornerHandler()
	:XercesBaseHandler()
{
	mAdorner = new Adorner();
}
//------------------------------------------------------------------------------

AdornerHandler::~AdornerHandler()
{
	delete mAdorner;
}
//------------------------------------------------------------------------------

void AdornerHandler::SetDocRef(unsigned int inNumDoc)
{
	mAdorner->SetAdornerDocRef(inNumDoc);
}
//------------------------------------------------------------------------------

void AdornerHandler::SetDbManager(DbManager *inDbMgr)
{
	mAdorner->SetDbManager(inDbMgr);
}
//------------------------------------------------------------------------------

void AdornerHandler::EnableOutputString(bool inEnable)
{
	mAdorner->EnableAdornerString(inEnable);
}
//------------------------------------------------------------------------------

void AdornerHandler::EnableOutputRecordBreaker(bool inEnable)
{
	mAdorner->EnableAdornerRecordBreaker(inEnable);
}
//------------------------------------------------------------------------------

void AdornerHandler::LoadRecordBreakerConfig(const std::string &inFileName)
{
	mAdorner->LoadAdornerRecordBreakerConfig(inFileName);
}
//------------------------------------------------------------------------------

UnicodeString &AdornerHandler::GetOutputString()
{
	return mAdorner->GetAdornerString();
}
//------------------------------------------------------------------------------

void AdornerHandler::SetDstFileName(const std::string &inFileName)
{
	mAdorner->SetAdornerDstFileName(inFileName);
}
//------------------------------------------------------------------------------

bool AdornerHandler::InitHandler()
{
	bool isOk = true;
	
	if (mAdorner->GetNumDoc() == 0) {
		gLog.log(eTypLogError, "Err > AdornerHandler: Document number is not set");
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

void AdornerHandler::BeginDocument()
{
	// Call the parent
	XercesBaseHandler::BeginDocument();

	mAdorner->StartDocument();
}
//------------------------------------------------------------------------------

void AdornerHandler::EndDocument(bool inIsException)
{
	mAdorner->EndDocument(inIsException);

	// Add adorner errors
	AddNbError(mAdorner->GetNbError());

	// Call the parent
	XercesBaseHandler::EndDocument(inIsException);
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void AdornerHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const inLocalName,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update current element and attributes map
	XercesBaseHandler::startElement(inUri, inLocalName, inQName, inAttrs);

	// Send the start element notification to the adorner
	mAdorner->StartNode(mElem, mAttrMap);
}
//------------------------------------------------------------------------------

void AdornerHandler::endElement(const XMLCh * const inUri,
																const XMLCh * const inLocalName,
																const XMLCh * const inQName)
{
	// Send the end element notification to the adorner
	mAdorner->EndNode();

	XercesBaseHandler::endElement(inUri, inLocalName, inQName);
}
//------------------------------------------------------------------------------

void AdornerHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	// Send the characters notification to the adorner
	const UChar *str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	mAdorner->AddCharacters(str, (unsigned int)inLength);
}
//------------------------------------------------------------------------------

void AdornerHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	// Call the SAX character handler
	characters(inStr, inLength);
}
//------------------------------------------------------------------------------

void AdornerHandler::processingInstruction(const XMLCh *const inTarget, const XMLCh *const inData)
{
	const UChar *piStr = mCnv->ConvertToUChar(inTarget);
	mAdorner->ProcessingInstruction(piStr);
}
//------------------------------------------------------------------------------
