/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesNode.hpp

*******************************************************************************/

#ifndef __XercesNode_HPP
#define __XercesNode_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include "Containers.hpp"
#if defined(_MSC_VER)
#pragma warning(disable : 4251) // warning: class 'std::vector<_Ty>' nécessite une interface DLL pour être utilisé(e) par les clients de class
#endif // !_MSC_VER
//------------------------------------------------------------------------------

namespace Common
{

//! Xerces storage of an xml node, with element and attributes
/**
	Logic:
		- Store the content of an xml node
*/
class DEF_Export XercesNode
{
public:
	 XercesNode(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap, int inTypToken);
	~XercesNode();

	int GetTypToken() const { return mTypToken; }
	//! Tells if there is attributes
	bool IsAttr() const;
	//! Tells if a specific attribute is set. Example: IsAttr("class", "image")
	bool IsAttr(const std::string &inAttr, const UnicodeString &inAttrValue) const;
	void ToString(UnicodeString &outStr, bool inOpen) const;
	const std::string &GetElem() const { return mElem; }
	const UnicodeString &GetAttrs() const { return mAttrs; }

private:
	int mTypToken;
	std::string mElem;
	UnicodeString mAttrs;
};

typedef std::vector <Common::XercesNode *> XercesNodeVector;
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
