/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigHandler.hpp

*******************************************************************************/

#ifndef __IndexerConfigHandler_HPP
#define __IndexerConfigHandler_HPP
//------------------------------------------------------------------------------

#include "../common/XercesBaseHandler.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{
class IndexerConfig;

//! Xerces parser handler for the xml IndexerConfig
/**
	Logic:
		- Receive notification of parser events and send it to the xml IndexerConfig
		- Xerces strings are converted in unicode strings
	Xerces API:
		http://xerces.apache.org/xerces-c/apiDocs-3/classDefaultHandler.html
*/
class IndexerConfigHandler : public Common::XercesBaseHandler
{
private:
	IndexerConfig *mIndexerConfig;

public:
	IndexerConfigHandler();
	virtual ~IndexerConfigHandler();

	//! Init the indexer configuration. Must be set before parsing
	void SetIndexerConfig(IndexerConfig *inIndexerConfig);

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
