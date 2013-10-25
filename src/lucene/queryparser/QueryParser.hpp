/*******************************************************************************

QueryParser.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __QueryParser_HPP
#define __QueryParser_HPP
//------------------------------------------------------------------------------

#include "QueryToken.hpp"
#include "QueryParserBase.hpp"
//------------------------------------------------------------------------------

CL_NS_DEF(queryParser)
class TokenList;
CL_NS_END

CL_NS_DEF(search)
class Query;
CL_NS_END

CL_NS_DEF(util)
class Reader;
CL_NS_END

CL_NS_DEF(queryParser)

/**
* @brief Atoll's default query parser
*
* <p>It's a query parser.
* The only method that clients should need to call is Parse().
* The syntax for query const UChar*s is as follows:
* A Query is a series of clauses. A clause may be prefixed by:</p>
* <ul>
*	<li>a plus (+) or a minus (-) sign, indicating that the
*	clause is required or prohibited respectively; or</li>
*	<li>a term followed by a colon, indicating the field to be searched.
*	This enables one to construct queries which search multiple fields.</li>
*	</ul>
*	<p>
*	A clause may be either:</p>
*	<ul>
*	<li>a term, indicating all the documents that contain this term; or</li>
*	<li>a nested query, enclosed in parentheses. Note that this may be
*	used with a +/- prefix to require any of a set of terms.</li>
*	</ul>
*	<p>
* Thus, in BNF, the query grammar is:</p>
*	<code>
*	Query  ::= (Clause)*
*	Clause ::= ["+", "-"] [&lt;TERM&gt; ":"] (&lt;TERM&gt; | "(" Query ")")
*	</code>
*	<p>
*	Examples of appropriately formatted queries can be found in the test cases.
*	</p>
*/
class QueryParser : public QueryParserBase
{
private:
	const UChar *mField;
	TokenList *mTokenList;

	/**
	* matches for CONJUNCTION
	* CONJUNCTION ::= <AND> | <OR>
	*/
	int32_t MatchConjunction();

	/**
	* matches for MODIFIER
	* MODIFIER ::= <PLUS> | <MINUS> | <NOT>
	*/
	int32_t MatchModifier();

	/**
	* matches for QUERY
	* QUERY ::= [MODIFIER] CLAUSE (<CONJUNCTION> [MODIFIER] CLAUSE)*
	*/
	CL_NS(search)::Query *MatchQuery(const UChar *field);

	/**
	* matches for CLAUSE
	* CLAUSE ::= [TERM <COLON>] ( TERM | (<LPAREN> QUERY <RPAREN>))
	*/
	CL_NS(search)::Query *MatchClause(const UChar *field);

	/**
	* matches for TERM
	* TERM ::= TERM | PREFIXTERM | WILDTERM | NUMBER
	*  		[ <FUZZY> ] [ <CARAT> <NUMBER> [<FUZZY>]]
	*
	*  		| (<RANGEIN> | <RANGEEX>) [<CARAT> <NUMBER>]
	*  		| <QUOTED> [SLOP] [<CARAT> <NUMBER>]
	*/
	CL_NS(search)::Query *MatchTerm(const UChar *field);

	/**
	* matches for QueryToken of the specified type and returns it
	* otherwise Exception throws
	*/
	QueryToken *MatchQueryToken(QueryToken::Types expectedType);

	/**
	 * Extracts the first token from the Tokenlist tokenlist and destroys it
	 */
	void ExtractAndDeleteToken(void);

public:
	/**
	 * Initializes a new instance of the QueryParser class with a specified field and analyzer values
	 */
	QueryParser(const UChar *field, CL_NS(analysis)::Analyzer *analyzer);
	~QueryParser();

	/**
	* Returns a parsed Query instance.
	* Note: this call is not threadsafe, either use a seperate QueryParser for each thread, or use a thread lock
	* @param query The query string to be parsed
	* @param inLength The length of the query string
	*
	* @return A parsed Query instance
	*/
	virtual CL_NS(search)::Query *parse(const UChar *query, int32_t inLength = -1);

	/**
	* Returns a parsed Query instance.
	* Note: this call is not threadsafe, either use a seperate QueryParser for each thread, or use a thread lock
	* @param reader The TextReader to be parsed
	*
	* @return A parsed Query instance
	*/
	virtual CL_NS(search)::Query *parse(CL_NS(util)::Reader *reader);

	/**
	* Returns a new instance of the Query class with a specified query, field and analyzer values
	*/
	static CL_NS(search)::Query *parse(const UChar *query, const UChar *field, CL_NS(analysis)::Analyzer *analyzer);

	CL_NS(analysis)::Analyzer *getAnalyzer() {
		return analyzer;
	}

	/**
	* @return Returns the field.
	*/
	const UChar *getField() {
		return mField;
	}
};
//------------------------------------------------------------------------------

CL_NS_END

//------------------------------------------------------------------------------
#endif
