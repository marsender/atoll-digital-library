/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesBaseHandler.hpp

*******************************************************************************/

#ifndef __XercesBaseHandler_HPP
#define __XercesBaseHandler_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <string>
#include <xercesc/sax2/DefaultHandler.hpp>
#include "Containers.hpp"
//------------------------------------------------------------------------------

#if _XERCES_VERSION >= 30000
	typedef XMLSize_t XmlLength;
#else
	typedef unsigned int XmlLength;
#endif

XERCES_CPP_NAMESPACE_USE

namespace Common
{
class XercesString;

//! Xerces parser base handler
class XercesBaseHandler : public DefaultHandler
{
private:
	bool mTraceError; //! Trace errors in the document ?
	unsigned long mNbError;	//!< Number of errors in the document
	unsigned int mDepth;
	unsigned int mAttrCount;
	unsigned int mCharacterCount;
	unsigned int mElementCount;
	unsigned int mSpaceCount;

	enum eTypParseError {
		eTypParseErrorNone,
		eTypParseErrorFatal,
		eTypParseErrorError,
		eTypParseErrorWarning,
		eTypParseErrorEnd
	};

	void OutputError(const SAXParseException &e, eTypParseError inTypError);

protected:
	Common::XercesString *mCnv;
	std::string mElem;
	Common::StringToUnicodeStringMap mAttrMap;

public:
	DEF_Export XercesBaseHandler();
	DEF_Export virtual ~XercesBaseHandler();

	// -----------------------------------------------------------------------
	//  Getter methods
	// -----------------------------------------------------------------------
	DEF_Export unsigned int GetAttrCount() const { return mAttrCount; }

	DEF_Export unsigned int GetCharacterCount() const { return mCharacterCount; }

	DEF_Export unsigned int GetElementCount() const { return mElementCount; }

	DEF_Export unsigned int GetSpaceCount() const { return mSpaceCount; }

	DEF_Export unsigned long GetNbError() const { return mNbError; }
	DEF_Export void AddNbError(unsigned long inNbError) { mNbError += inNbError; }

	DEF_Export bool GetTraceError() const { return mTraceError; }
	DEF_Export void SetTraceError(bool inTraceError) { mTraceError = inTraceError; }

  DEF_Export virtual void BeginDocument();
  DEF_Export virtual void EndDocument(bool inIsException);

	// -----------------------------------------------------------------------
	//  Handlers for the SAX DocumentHandler interface
	// -----------------------------------------------------------------------
  DEF_Export virtual void startDocument();
	//! Receive notification of the start of an namespace prefix mapping
	DEF_Export virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);
	//! Receive notification of the start of an element
	DEF_Export virtual void startElement(const XMLCh * const uri, const XMLCh * const localname,
										const XMLCh * const qname, const Attributes &attrs);
	//! Receive notification of the end of an element
	DEF_Export virtual void endElement(const XMLCh * const uri, const XMLCh * const localname,
										const XMLCh * const qname);
	DEF_Export virtual void characters(const XMLCh * const chars, const XmlLength length);
	DEF_Export virtual void ignorableWhitespace(const XMLCh * const chars, const XmlLength length);
	DEF_Export virtual void endDocument();

	// -----------------------------------------------------------------------
	//  Handlers for the SAX ErrorHandler interface
	// -----------------------------------------------------------------------
	DEF_Export virtual void fatalError(const SAXParseException &e);
	DEF_Export virtual void error(const SAXParseException &e);
	DEF_Export virtual void warning(const SAXParseException &e);
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
