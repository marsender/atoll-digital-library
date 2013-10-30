/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesNode.cpp

*******************************************************************************/

#include "XercesNode.hpp"
#include "UnicodeUtil.hpp"
#include "unicode/ustring.h" // u_strstr, u_strchr...
//------------------------------------------------------------------------------

using namespace Common;

XercesNode::XercesNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap, int inTypToken)
{
	mTypToken = inTypToken;
	mElem = inElem;

	// Store the attributes
	if (mTypToken > 0 && inAttrMap.size()) {
		int nbAttr = 0;
		const std::string *attr;
		UnicodeString *uStr;
		const UChar *str;
		StringToUnicodeStringMap::const_iterator it = inAttrMap.begin();
		StringToUnicodeStringMap::const_iterator itEnd = inAttrMap.end();
		for (; it != itEnd; ++it) {
			if (nbAttr++)
				mAttrs += " ";
			attr = &it->first;
			mAttrs += attr->c_str();
			mAttrs += "=\"";
			uStr = (UnicodeString *)&it->second; // ConstHack
			str = uStr->getTerminatedBuffer();
			if (u_strchr(str, '"')) {
				uStr->findAndReplace("\"", "'");
				str = uStr->getTerminatedBuffer();
			}
			UnicodeStringAddXmlEntities(mAttrs, str);
			mAttrs += "\"";
		}
	}
}
//------------------------------------------------------------------------------

XercesNode::~XercesNode()
{
}
//------------------------------------------------------------------------------

bool XercesNode::IsAttr() const
{
	bool isAttr = mAttrs.length() > 0;

	return isAttr;
}
//------------------------------------------------------------------------------

bool XercesNode::IsAttr(const std::string &inAttr, const UnicodeString &inAttrValue) const
{
	bool isAttr;
	
	isAttr = mAttrs.indexOf(inAttrValue) != -1;
	if (!isAttr)
		return false;

	UnicodeString str = inAttr.c_str();
	str += "=\"";
	str += inAttrValue;
	str += "\"";
	isAttr = mAttrs.indexOf(str) != -1;

	return isAttr;
}
//------------------------------------------------------------------------------

void XercesNode::ToString(UnicodeString &outStr, bool inOpen) const
{
	outStr.remove();

	if (mTypToken == 0)
		return;

	outStr += "<";
	if (!inOpen)
		outStr += "/";
	outStr += mElem.c_str();
	if (inOpen && mAttrs.length()) {
		outStr += " ";
		outStr += mAttrs;
	}
	outStr += ">";

	return;
}
//------------------------------------------------------------------------------
