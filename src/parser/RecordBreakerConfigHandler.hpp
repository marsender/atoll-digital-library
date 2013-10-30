/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfigHandler.hpp

*******************************************************************************/

#ifndef __RecordBreakerConfigHandler_HPP
#define __RecordBreakerConfigHandler_HPP
//------------------------------------------------------------------------------

#include "../common/XercesBaseHandler.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{
class RecordBreakerConfig;

//! Xerces parser handler for the xml RecordBreakerConfig
/**
	Logic:
		- Receive notification of parser events and send it to the xml RecordBreakerConfig
		- Xerces strings are converted in unicode strings
	Xerces API:
		http://xerces.apache.org/xerces-c/apiDocs-3/classDefaultHandler.html
*/
class RecordBreakerConfigHandler : public Common::XercesBaseHandler
{
private:
	RecordBreakerConfig *mRecordBreakerConfig;

public:
	RecordBreakerConfigHandler();
	virtual ~RecordBreakerConfigHandler();

	//! Init the record breaker configuration. Must be set before parsing
	void SetRecordBreakerConfig(RecordBreakerConfig *inRecordBreakerConfig);

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

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
