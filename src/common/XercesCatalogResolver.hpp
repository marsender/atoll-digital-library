/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesCatalogResolver.hpp

*******************************************************************************/

#ifndef __XercesCatalogResolver_HPP
#define __XercesCatalogResolver_HPP
//------------------------------------------------------------------------------

#include <string>
#include <map>
#include <xercesc/parsers/XercesDOMParser.hpp>
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace Common
{

//! Xerces xml entity and uri resolver with a catalog file
/**
	Logic:
		- See url: http://xml.apache.org/commons/components/resolver/resolver-article.html
*/
class XercesCatalogResolver
{
private:
	typedef std::map < std::string, std::string > CatalogMap;

	bool mInitializeXerces;
	CatalogMap mCatalogMap;
	XercesDOMParser *mParser;

	void InitXercesParser();
	void CloseXercesParser();
	bool ReadCatalog();

public:
	XercesCatalogResolver(bool inInitializeXerces = false);
	virtual ~XercesCatalogResolver();

	bool LoadCatalog(const std::string &inFileName);
	std::string LookupPublicId(const std::string &inPublicId);
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
