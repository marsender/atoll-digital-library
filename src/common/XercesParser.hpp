/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesParser.hpp

*******************************************************************************/

#ifndef __XercesParser_HPP
#define __XercesParser_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp> // SAX2XMLReader::ValSchemes
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace Common
{
class XercesBaseHandler;
class XercesEntityResolver;

//! Xerces parser base class
class XercesParser
{
private:
	std::string mCatalogFile;
	static std::string mStaticDefaultCatalogFile;
	SAX2XMLReader::ValSchemes mValScheme;

protected:
	SAX2XMLReader *mParser;
	XercesBaseHandler *mHandler;
	XercesEntityResolver *mEntityResolver;

	void InitXercesParser();
	DEF_Export void InitXercesHandler();
	void CloseXercesParser();

public:
	DEF_Export XercesParser(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile);
	DEF_Export virtual ~XercesParser();

	void *GetHandler() { return mHandler; }
	DEF_Export static const std::string &StaticGetDefaultCatalogFile();
	virtual bool XercesParse(const std::string &inFileName) = 0;
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
