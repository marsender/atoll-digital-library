/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordEntry.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "WordEntry.hpp"
#include "../common/Containers.hpp"
#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const WordEntry &inWordEntry)
{
	inStream << inWordEntry.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

WordEntry::WordEntry()
{
}
//------------------------------------------------------------------------------

WordEntry::WordEntry(const UChar *inStr, const Entry &inEntry)
{
	if (inStr)
		mWrd = inStr;
	mEntry = inEntry;
}
//------------------------------------------------------------------------------

WordEntry::WordEntry(const UnicodeString &inStr, const Entry &inEntry)
{
	mWrd = inStr;
	mEntry = inEntry;
}
//------------------------------------------------------------------------------

std::string WordEntry::ToString() const
{
	std::string s("WordEntry: ");

	s += UnicodeString2String(mWrd);
	s += " [";
	s += mEntry.ToString();
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
