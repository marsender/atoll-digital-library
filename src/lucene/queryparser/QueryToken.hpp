/*******************************************************************************

QueryToken.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __QueryToken_HPP
#define __QueryToken_HPP
//------------------------------------------------------------------------------

CL_NS_DEF(queryParser)

//! Query token that is used by the query parser
class QueryToken
{
public:

	enum Types {
		AND_,
		OR,
		NOT,
		PLUS,
		MINUS,
		LPAREN,
		RPAREN,
		COLON,
		CARAT,
		QUOTED,
		TERM,
		SLOP,
		FUZZY,
		PREFIXTERM,
		WILDTERM,
		RANGEIN,
		RANGEEX,
		FULLFIELD,
		NUMBER,
		EOF_,
		UNKNOWN_
	};

	UChar *mValue;
	int32_t mStart;
	int32_t mEnd;
	QueryToken::Types mType;

	//! Initializes a new instance of the QueryToken class
	QueryToken(const UChar *inValue, int32_t inStart, int32_t inEnd, QueryToken::Types inType);
	//! Initializes a new instance of the QueryToken class
	QueryToken(const UChar *inValue, QueryToken::Types inType);
	//! Initializes a new instance of the QueryToken class
	QueryToken(QueryToken::Types inType);
	//! Initializes a new instance of the QueryToken class
	QueryToken();
	~QueryToken();

	void set(const UChar *inValue, int32_t inStart, int32_t inEnd, QueryToken::Types inType);
	void set(const UChar *inValue, QueryToken::Types inType);
	void set(QueryToken::Types inType);
};
//------------------------------------------------------------------------------

CL_NS_END

//------------------------------------------------------------------------------
#endif
