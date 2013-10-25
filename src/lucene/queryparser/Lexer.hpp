/*******************************************************************************

Lexer.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __Lexer_HPP
#define __Lexer_HPP
//------------------------------------------------------------------------------

#include "QueryToken.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

CL_NS_DEF(util)
class Reader;
class FastCharStream;
CL_NS_END

CL_NS_DEF(queryParser)

class QueryParserBase;
class QueryToken;
class TokenList;

//! Lexer that is used by the query parser
class Lexer
{
private:
	bool delSR;  //Indicates if the reader must be deleted or not
	QueryParserBase* queryparser; //holds the queryparser so that we can do callbacks
	CL_NS(util)::FastCharStream* reader;

	//! Get the next token
	bool GetNextToken(QueryToken *outQueryToken);

	//! Reads an integer number. Output string should be as large as a field can be
	void ReadIntegerNumber(const UChar inChar, UChar *outStr, int32_t inLength);

	//! Reads an inclusive range like [some words]
	bool ReadInclusiveRange(const UChar inCharPrev, QueryToken *outQueryToken);

	//! Reads an exclusive range like {some words}
	bool ReadExclusiveRange(const UChar inCharPrev, QueryToken *outQueryToken);

	//! Reads a full field between a lower and an upper sign like <some words>
	bool ReadFullField(const UChar inCharPrev, QueryToken *outQueryToken);

	//! Reads quoted string like "something else"
	bool ReadQuoted(const UChar inCharPrev, QueryToken *outQueryToken);

	//! Reads a term
	bool ReadTerm(const UChar inCharPrev, QueryToken *outQueryToken);

	//! Reads an escaped character into the buf. Buf requires at least 3 characters
	bool ReadEscape(const UChar inCharPrev, UChar *outBuf);

public:
	//! Initializes a new instance of the Lexer class with the specified query to lex.
	Lexer(QueryParserBase* queryparser, const UChar* query);

	//! Initializes a new instance of the Lexer class with the specified TextReader to lex.
	Lexer(QueryParserBase* queryparser, CL_NS(util)::Reader* source);

	~Lexer();

	//! Init string declarations
	static void InitStringDecl();

	//! Breaks the input stream onto the list of query tokens
	void Lex(TokenList *outTokenList);
};
//------------------------------------------------------------------------------

CL_NS_END

//------------------------------------------------------------------------------
#endif
