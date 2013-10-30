/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesEntityResolver.hpp

*******************************************************************************/

#ifndef __XercesEntityResolver_HPP
#define __XercesEntityResolver_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <string>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp> // SAX2XMLReader::ValSchemes
#include <xercesc/util/XMLEntityResolver.hpp>
#if defined(_MSC_VER)
#pragma warning(disable : 4251) // warning: class 'std::vector<_Ty>' nécessite une interface DLL pour être utilisé(e) par les clients de class
#endif // !_MSC_VER
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace Common
{

//! Xerces entity resolver
class DEF_Export XercesEntityResolver : public EntityResolver, public XMLEntityResolver
{
public:
	XercesEntityResolver(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile);
	virtual ~XercesEntityResolver();

	// -----------------------------------------------------------------------
	//  Handlers for the SAX EntityResolver interface
	// -----------------------------------------------------------------------
	virtual InputSource *resolveEntity(const XMLCh * const inPublicId, const XMLCh * const inSystemId);
	virtual InputSource *resolveEntity(XMLResourceIdentifier *inResourceIdentifier);

private:
	std::string mCatalogFile;
	SAX2XMLReader::ValSchemes mValScheme;

	// No need for copy and assignment
	XercesEntityResolver(const XercesEntityResolver &);
	XercesEntityResolver &operator=(const XercesEntityResolver &);
	bool operator==(const XercesEntityResolver&) const;
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
