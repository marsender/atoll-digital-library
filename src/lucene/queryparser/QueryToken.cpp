/*******************************************************************************

QueryToken.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "QueryToken.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

const UChar cBlankString[] = { 0 };
//------------------------------------------------------------------------------

CL_NS_DEF(queryParser)

QueryToken::QueryToken(const UChar *inValue, int32_t inStart, int32_t inEnd, QueryToken::Types inType)
	:mValue(NULL)
{
	set(inValue, inStart, inEnd, inType);
}
//------------------------------------------------------------------------------

QueryToken::QueryToken(const UChar *inValue, QueryToken::Types inType)
	:mValue(NULL)
{
	set(inValue, inType);
}
//------------------------------------------------------------------------------

QueryToken::QueryToken(QueryToken::Types inType)
	:mValue(NULL)
{
	set(inType);
}
//------------------------------------------------------------------------------

QueryToken::QueryToken()
	:mValue(NULL)
{
	set(QueryToken::UNKNOWN_);
}
//------------------------------------------------------------------------------

QueryToken::~QueryToken()
{
	if (mValue)
		delete [] mValue;
}
//------------------------------------------------------------------------------

void QueryToken::set(const UChar *inValue, int32_t inStart, int32_t inEnd, QueryToken::Types inType)
{
	if (mValue)
		delete [] mValue;
	if (inEnd == -1)
		inEnd = u_strlen(inValue);
	mValue = new UChar[inEnd + 1];
	u_strncpy(mValue, inValue, inEnd);
	mValue[inEnd] = 0;

	this->mStart = inStart;
	this->mEnd = inEnd;
	this->mType = inType;
}
//------------------------------------------------------------------------------

void QueryToken::set(const UChar *inValue, QueryToken::Types inType)
{
	set(inValue, 0, -1, inType);
}
//------------------------------------------------------------------------------

void QueryToken::set(QueryToken::Types inType)
{
	set(cBlankString, 0, 0, inType);
}
//------------------------------------------------------------------------------

CL_NS_END
