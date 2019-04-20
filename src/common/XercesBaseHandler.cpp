/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesBaseHandler.cpp

*******************************************************************************/

#include "XercesBaseHandler.hpp"
#include "FileSystem.hpp" // in common directory
#include "Logger.hpp" // in common directory
#include "XercesCatalogResolver.hpp" // in common directory
#include "XercesString.hpp" // in common directory
#include <xercesc/sax2/Attributes.hpp>
//#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <string>
#include <sstream> // For std::ostringstream 
//------------------------------------------------------------------------------

using namespace Common;
XERCES_CPP_NAMESPACE_USE

XercesBaseHandler::XercesBaseHandler()
{
	mDepth = 0;
	mNbError = 0;
	mElementCount = 0;
	mAttrCount = 0;
	mCharacterCount = 0;
	mSpaceCount = 0;

	mCnv = new XercesString();
}
//------------------------------------------------------------------------------

XercesBaseHandler::~XercesBaseHandler()
{
	delete mCnv;

	// Test
	//if (mNbError)
	//	mNbError += 0;
}
//------------------------------------------------------------------------------

void XercesBaseHandler::BeginDocument()
{
	mNbError = 0;
	mDepth = 0;
	mAttrCount = 0;
	mCharacterCount = 0;
	mElementCount = 0;
	mSpaceCount = 0;
	mElem = "";
	mAttrMap.clear();
}
//------------------------------------------------------------------------------

void XercesBaseHandler::EndDocument(bool inIsException)
{
	// Make sure that the number of errors is set if there is an exception
	if (inIsException && mNbError == 0)
		mNbError++;
}
//------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Handlers for the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

void XercesBaseHandler::startPrefixMapping(const XMLCh* const inPrefix, const XMLCh* const inUri)
{
}

void XercesBaseHandler::startElement(const XMLCh * const inUri,
																		const XMLCh * const /*inLocalName*/,
																		const XMLCh * const inQName,
																		const Attributes &inAttrs)
{
	// Update the current depth
	mDepth++;

	mElementCount++;

	// Store element
	XercesString elemXerces(inQName);
	mElem = elemXerces.localForm();

	// Store attributes
	mAttrMap.clear();
	// Add the namespace if this is the root node
	if (mDepth == 1 && inUri && *inUri) {
		std::string attrName;
		UnicodeString attrValue;
		XercesString attrNameXerces(XMLUni::fgXMLNSString);
		attrName = attrNameXerces.localForm();
		attrValue = mCnv->ConvertToUChar(inUri);
		mAttrMap[attrName] = attrValue;
	}
	mAttrCount = (unsigned int)inAttrs.getLength();
	if (mAttrCount) {
		std::string attrName;
		UnicodeString attrValue;
		for (unsigned int numAttr = 0; numAttr < mAttrCount; ++numAttr) {
			XercesString attrNameXerces(inAttrs.getQName(numAttr));
			attrName = attrNameXerces.localForm();
			attrValue = mCnv->ConvertToUChar(inAttrs.getValue(numAttr));
			mAttrMap[attrName] = attrValue;
		}
	}
}
//------------------------------------------------------------------------------

void XercesBaseHandler::endElement(const XMLCh * const /*inUri*/,
														 const XMLCh * const /*inLocalName*/,
														 const XMLCh * const /*inQName*/)
{
	// Update the current depth
	mDepth--;
}
//------------------------------------------------------------------------------

void XercesBaseHandler::characters(const XMLCh * const inStr, const XmlLength inLength)
{
	mCharacterCount += (unsigned int)inLength;

	UnicodeString str;
	str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	//const UChar *sDebug = str.getTerminatedBuffer();
}
//------------------------------------------------------------------------------

void XercesBaseHandler::ignorableWhitespace(const XMLCh * const inStr, const XmlLength inLength)
{
	mSpaceCount += (unsigned int)inLength;

	UnicodeString str;
	str = mCnv->ConvertToUChar(inStr, (unsigned int)inLength);
	//const UChar *sDebug = str.getTerminatedBuffer();
}
//------------------------------------------------------------------------------

// Receive notification of the beginning of the document
void XercesBaseHandler::startDocument()
{
	// Test
	//if (mNbError)
	//	mNbError += 0;
}
//------------------------------------------------------------------------------

// Receive notification of the end of the document
void XercesBaseHandler::endDocument()
{
	// Test
	//if (mNbError)
	//	mNbError += 0;
}
//------------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Handlers for the SAX ErrorHandler interface
// ---------------------------------------------------------------------------

void XercesBaseHandler::OutputError(const SAXParseException &e, eTypParseError inTypError)
{
	eTypLog typLog;
	char strLog[10];
	std::string typError;
	std::ostringstream oss;

	if (mTraceError == false && inTypError != eTypParseErrorFatal)
		return;

	switch (inTypError) {
	case eTypParseErrorFatal:
		mNbError++;
		strcpy(strLog, "Fat");
		typError = "Xerces handler fatal error";
		typLog = eTypLogFatal;
		break;
	case eTypParseErrorError:
		mNbError++;
		strcpy(strLog, "Err");
		typError = "Xerces handler error";
		typLog = eTypLogError;
		break;
	case eTypParseErrorWarning:
		strcpy(strLog, "Inf");
		typError = "Xerces handler warning";
		typLog = eTypLogWarning;
		break;
	default:
		mNbError++;
		strcpy(strLog, "Err");
		typError = "Xerces handler undefined error";
		typLog = eTypLogError;
		break;
	}

	XercesString systemId(e.getSystemId());
	XercesString message(e.getMessage());

	const char *sSystemId = systemId.localForm();
	const char *sMessage = message.localForm();
	long line = (long)e.getLineNumber();
	long column = (long)e.getColumnNumber();

	// Logs the error message
	gLog.log(typLog, "%s > %s(%ld:%ld): %s: %s", strLog,
		sSystemId ? sSystemId : "No system id",
		line, column, typError.c_str(),
		sMessage ? sMessage : "No error message");
}
//------------------------------------------------------------------------------

void XercesBaseHandler::fatalError(const SAXParseException &e)
{
	OutputError(e, eTypParseErrorFatal);
}
//------------------------------------------------------------------------------

void XercesBaseHandler::error(const SAXParseException &e)
{
	OutputError(e, eTypParseErrorError);
}
//------------------------------------------------------------------------------

void XercesBaseHandler::warning(const SAXParseException &e)
{
	OutputError(e, eTypParseErrorWarning);
}
//------------------------------------------------------------------------------
