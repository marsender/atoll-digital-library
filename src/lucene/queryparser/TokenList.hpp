/*******************************************************************************

TokenList.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __TokenList_HPP
#define __TokenList_HPP
//------------------------------------------------------------------------------

#include <vector>
//------------------------------------------------------------------------------

CL_NS_DEF(queryParser)

class QueryToken;

//! List of query tokens
class TokenList
{
private:
	typedef std::vector < QueryToken * > QueryTokenVector;
	QueryTokenVector *mQueryTokenVector;

public:
	//! Initializes a new instance of the TokenList class
	TokenList();
	~TokenList();

	void Add(QueryToken *inQueryToken); //!< Add an element at the bebining
	void Push(QueryToken *inQueryToken); //!< Add an element at the end
	QueryToken *Peek(); //!< Get last element
	QueryToken *Extract(); //!< Get and remove last element
	int32_t Count() const; //!< Get the number of elements
};
//------------------------------------------------------------------------------

CL_NS_END

//------------------------------------------------------------------------------
#endif
