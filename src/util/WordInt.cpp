/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordInt.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "WordInt.hpp"
#include "../common/Containers.hpp"
#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const WordInt &inWordInt)
{
	inStream << inWordInt.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

WordInt::WordInt()
{
	mInt = 0;
}
//------------------------------------------------------------------------------

WordInt::WordInt(const UChar *inStr, unsigned long inInt)
{
	if (inStr)
		mWrd = inStr;
	mInt = inInt;
}
//------------------------------------------------------------------------------

WordInt::WordInt(const UnicodeString &inStr, unsigned long inInt)
{
	mWrd = inStr;
	mInt = inInt;
}
//------------------------------------------------------------------------------

std::string WordInt::ToString() const
{
	char buf[11]; // 4294967295
	std::string s("WordInt: ");

	sprintf(buf, "%lu", mInt);
	s += UnicodeString2String(mWrd);
	s += " [";
	s += buf;
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
