/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Parser.hpp

*******************************************************************************/

#ifndef __Parser_HPP
#define __Parser_HPP
//------------------------------------------------------------------------------

#include "../common/XercesParser.hpp"
#include "unicode/ustring.h"
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace Atoll
{

//! Enum of handlers for the engine parser
enum eTypHandler {
	eTypHandlerNone,
	eTypHandlerBase,
	eTypHandlerIndexer,
	eTypHandlerAdorner,
	eTypHandlerIndexerConfig,
	eTypHandlerRecordBreakerConfig,
	eTypHandlerEnd
};

//! Xerces parser for atoll engine
class Parser : public Common::XercesParser
{
private:
	eTypHandler mTypHandler;

	bool XercesInitHandler();

public:
	Parser(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile, eTypHandler inTypHandler);
	virtual ~Parser();

	//! Parse an xml file
	bool XercesParse(const std::string &inFileName);
	//! Parse an xml buffer
	bool XercesParse(const UChar *inStr, int32_t inLength = -1);
};
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
