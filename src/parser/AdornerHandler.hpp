/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerHandler.hpp

*******************************************************************************/

#ifndef __AdornerHandler_HPP
#define __AdornerHandler_HPP
//------------------------------------------------------------------------------

#include "../common/XercesBaseHandler.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{
class Adorner;
class DbManager;

//! Xerces parser handler for the xml adorner
/**
	Logic:
		- Receive notification of parser events and send it to the xml adorner
		- Xerces strings are converted in unicode strings
	Xerces API:
		http://xerces.apache.org/xerces-c/apiDocs-3/classDefaultHandler.html
*/
class AdornerHandler : public Common::XercesBaseHandler
{
private:
	Adorner *mAdorner;

public:
	AdornerHandler();
	virtual ~AdornerHandler();

	//! Init the db manager. Must be set before parsing
	void SetDbManager(DbManager *inDbMgr);
	//! Enable the output string
	void EnableOutputString(bool inEnable);
	//! Get the handler output string
	UnicodeString &GetOutputString();

	//! Enable the output record breaker
	void EnableOutputRecordBreaker(bool inEnable);
	//! Load the record breaker configuration. Must be set before parsing
	void LoadRecordBreakerConfig(const std::string &inFileName);

	//! Init the document number. Must be set before parsing
	void SetDocRef(unsigned int inNumDoc);
	//! Init the destination file name. Must be set before parsing
	void SetDstFileName(const std::string &inFileName);

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
	virtual void processingInstruction(const XMLCh *const inTarget, const XMLCh *const inData);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
