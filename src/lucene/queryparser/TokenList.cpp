/*******************************************************************************

TokenList.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "TokenList.hpp"
#include "QueryToken.hpp"
//------------------------------------------------------------------------------

CL_NS_DEF(queryParser)

TokenList::TokenList()
{
	mQueryTokenVector = new QueryTokenVector;
}
//------------------------------------------------------------------------------

TokenList::~TokenList()
{
	//mQueryTokenVector->clear();
	delete mQueryTokenVector;
}
//------------------------------------------------------------------------------

void TokenList::Add(QueryToken *inQueryToken)
{
	AppAssert(inQueryToken != NULL);

	mQueryTokenVector->insert(mQueryTokenVector->begin(), inQueryToken);
}
//------------------------------------------------------------------------------

void TokenList::Push(QueryToken *inQueryToken)
{
	AppAssert(inQueryToken != NULL);

	mQueryTokenVector->push_back(inQueryToken);
}
//------------------------------------------------------------------------------

QueryToken *TokenList::Peek()
{
	size_t nTokens = mQueryTokenVector->size();
	if (nTokens == 0) {
		Push(new QueryToken(QueryToken::EOF_));
		//nTokens++;
	}

	return mQueryTokenVector->back(); //[nTokens-1];
}
//------------------------------------------------------------------------------

QueryToken *TokenList::Extract()
{
	QueryToken* token = Peek();

	// Retract the current peeked token
	mQueryTokenVector->pop_back();

	return token;
}
//------------------------------------------------------------------------------

int32_t TokenList::Count() const
{
	return mQueryTokenVector->size();
}
//------------------------------------------------------------------------------

CL_NS_END
