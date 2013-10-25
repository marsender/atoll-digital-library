/*******************************************************************************

QueryParser.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "QueryParser.hpp"
#include "TokenList.hpp"
#include "QueryParserBase.hpp"
#include "Lexer.hpp"
#include "../analysis/AnalysisHeader.hpp"
#include "../search/BooleanClause.hpp"
#include "../search/Query.hpp"
// #include "../index/Term.hpp"
#include "../util/StringBuffer.hpp"
#include "../util/Reader.hpp"
#include "../../common/UnicodeUtil.hpp"
#include <vector>
#include <cstdlib> // For strtod
// ------------------------------------------------------------------------------

CL_NS_USE(util)
CL_NS_USE(analysis)
CL_NS_USE(search)
// CL_NS_USE(index)
using namespace Common;

CL_NS_DEF(queryParser)

QueryParser::QueryParser(const UChar *_field, Analyzer *_analyzer) : QueryParserBase(_analyzer)
{
	// Func - Constructor.
	//        Instantiates a QueryParser for the named field _field
	// Pre  - _field != NULL
	// Post - An instance has been created

	if (_field)
		mField = STRDUP_TtoT(_field);
	else
		mField = NULL;
	mTokenList = NULL;
	lowercaseExpandedTerms = true;
}

QueryParser::~QueryParser()
{
	// Func - Destructor
	// Pre  - true
	// Post - The instance has been destroyed

	_CLDELETE_CARRAY(mField);
}

// static
Query *QueryParser::parse(const UChar *query, const UChar *field, Analyzer *analyzer)
{
	// Func - Returns a new instance of the Query class with a specified query, field and
	//        analyzer values.
	// Pre  - query != NULL and holds the query to parse
	//        field != NULL and holds the default field for query terms
	//        analyzer holds a valid reference to an Analyzer and is used to
	//        find terms in the query text
	// Post - query has been parsed and an instance of Query has been returned

	CND_PRECONDITION(query != NULL, "query is NULL");
	CND_PRECONDITION(field != NULL, "field is NULL");

	QueryParser parser(field, analyzer);
	return parser.parse(query);
}

Query *QueryParser::parse(const UChar *query, int32_t inLength /*= -1*/)
{
	// Func - Returns a parsed Query instance
	// Pre  - query != NULL and contains the query value to be parsed
	// Post - Returns a parsed Query Instance

	CND_PRECONDITION(query != NULL, "query is NULL");

	// Instanciate a reader that can read the query string
	int32_t len = inLength == -1 ? u_strlen(query) : inLength;
	StringReader *r = _CLNEW StringReader(query, len);

	// Pointer for the return value
	Query *ret = NULL;

	try {
		// Parse the query managed by the StringReader R and return a parsed Query instance into ret
		ret = parse(r);
	}
	_CLFINALLY (
	  _CLDELETE(r);
	);

	return ret;
}

Query *QueryParser::parse(Reader *reader)
{
	// Func - Returns a parsed Query instance
	// Pre  - reader contains a valid reference to a Reader and manages the query string
	// Post - A parsed Query instance has been returned or

	// instantiate the TokenList
	TokenList _tokens;
	mTokenList = &_tokens;

	// Instantiate a lexer
	Lexer lexer(this, reader);

	// Lex the tokens
	lexer.Lex(mTokenList);

	// Peek to the first token and check if is an EOF
	QueryToken::Types type = mTokenList->Peek()->mType;
	if (type == QueryToken::EOF_) {
		// The query string failed to yield any tokens.
		// Discard the TokenList and raise an exceptioin.
		QueryToken *token = mTokenList->Extract();
		_CLDELETE(token);
		_CLTHROWA(CL_ERR_Parse, "No query given.");
	}

	// Return the parsed Query instance
	Query *ret = MatchQuery(mField);
	mTokenList = NULL;
	return ret;
}

int32_t QueryParser::MatchConjunction()
{
	// Func - matches for CONJUNCTION
	//        CONJUNCTION ::= <AND> | <OR>
	// Pre  - mTokenList != NULL
	// Post - if the first token is an AND or an OR then
	//        the token is extracted and deleted and CONJ_AND or CONJ_OR is returned
	//        otherwise CONJ_NONE is returned

	CND_PRECONDITION(mTokenList != NULL, "Token list is NULL");

	QueryToken::Types type = mTokenList->Peek()->mType;
	switch (type) {
	case QueryToken::AND_:
		// Delete the first token of tokenlist
		ExtractAndDeleteToken();
		return CONJ_AND;
	case QueryToken::OR:
		// Delete the first token of tokenlist
		ExtractAndDeleteToken();
		return CONJ_OR;
	default :
		return CONJ_NONE;
	}
}

int32_t QueryParser::MatchModifier()
{
	// Func - matches for MODIFIER
	//        MODIFIER ::= <PLUS> | <MINUS> | <NOT>
	// Pre  - mTokenList != NULL
	// Post - if the first token is a PLUS the token is extracted and deleted and MOD_REQ is returned
	//        if the first token is a MINUS or NOT the token is extracted and deleted and MOD_NOT is returned
	//        otherwise MOD_NONE is returned
	CND_PRECONDITION(mTokenList != NULL, "Token list is NULL");

	QueryToken::Types type = mTokenList->Peek()->mType;
	switch (type) {
	case QueryToken::PLUS:
		// Delete the first token of tokenlist
		ExtractAndDeleteToken();
		return MOD_REQ;
	case QueryToken::MINUS:
	case QueryToken::NOT:
		// Delete the first token of tokenlist
		ExtractAndDeleteToken();
		return MOD_NOT;
	default :
		return MOD_NONE;
	}
}

Query *QueryParser::MatchQuery(const UChar *field)
{
	// Func - matches for QUERY
	//        QUERY ::= [MODIFIER] QueryParser::CLAUSE (<CONJUNCTION> [MODIFIER] CLAUSE)*
	// Pre  - field != NULL
	// Post -

	CND_PRECONDITION(mTokenList != NULL, "Token list is NULL");

	std::vector<BooleanClause*> clauses;

	Query *q = NULL;

	int32_t mods = MOD_NONE;
	int32_t conj = CONJ_NONE;

	// Match for MODIFIER
	mods = MatchModifier();

	// Match for CLAUSE
	q = MatchClause(field);
	AddClause(clauses, CONJ_NONE, mods, q);

	// Match for CLAUSE*
	QueryToken *token;
	while (true) {
		token = mTokenList->Peek();
		if (token->mType == QueryToken::EOF_) {
			QueryToken *qt = MatchQueryToken(QueryToken::EOF_);
			_CLDELETE(qt);
			break;
		}

		if (token->mType == QueryToken::RPAREN) {
			// MatchQueryToken(QueryToken::RPAREN);
			break;
		}

		// Match for a conjuction (AND OR NOT)
		conj = MatchConjunction();
		// Match for a modifier
		mods = MatchModifier();

		q = MatchClause(field);
		if (q != NULL)
			AddClause(clauses, conj, mods, q);
	}

	// Finalize query
	if (clauses.size() == 1) { // bvk: removed this && firstQuery != NULL
		BooleanClause *c = clauses[0];
		Query *q = c->query;

		// Condition check to be sure clauses[0] is valid
		CND_CONDITION(c != NULL, "c is NULL");

		// Tell the boolean clause not to delete its query
		c->deleteQuery = false;
		// Clear the clauses list
		clauses.clear();
		_CLDELETE(c);

		return q;
	}

	q = GetBooleanQuery(clauses);

	return q;
}

Query *QueryParser::MatchClause(const UChar *field)
{
	// Func - matches for CLAUSE
	//        CLAUSE ::= [TERM <COLONQueryParser::>] ( TERM | (<LPAREN> QUERY <RPAREN>))
	// Pre  - field != NULL
	// Post -

	Query *q = NULL;
	const UChar *sfield = field;
	bool delField = false;
	QueryToken *delToken = NULL; // Token that is about to be deleted
	QueryToken::Types type;

	// Match for [TERM <COLON>]
	QueryToken *token = mTokenList->Extract();
	type = mTokenList->Peek()->mType;
	if (token->mType == QueryToken::TERM && type == QueryToken::COLON) {
		delToken = MatchQueryToken(QueryToken::COLON);

		CND_CONDITION(delToken != NULL, "DelToken is NULL");
		_CLDELETE(delToken);

		UChar *tmp = STRDUP_TtoT(token->mValue);
		discardEscapeChar(tmp);
		delField = true;
		sfield = tmp;
		_CLDELETE(token);
	}
	else {
		mTokenList->Push(token);
		token = NULL;
	}

	// Match for TERM | (<LPAREN> QUERY <RPAREN>)
	type = mTokenList->Peek()->mType;
	if (type == QueryToken::LPAREN) {
		delToken = MatchQueryToken(QueryToken::LPAREN);

		CND_CONDITION(delToken != NULL, "DelToken is NULL");
		_CLDELETE(delToken);

		q = MatchQuery(sfield);
		// DSR:2004.11.01:
		// If exception is thrown while trying to match trailing parenthesis,
		// need to prevent q from leaking.

		try {
			delToken = MatchQueryToken(QueryToken::RPAREN);

			CND_CONDITION(delToken != NULL, "DelToken is NULL");
			_CLDELETE(delToken);

		}
		catch (...) {
			_CLDELETE(q);
			throw;
		}
	}
	else {
		q = MatchTerm(sfield);
	}

	if (delField)
		_CLDELETE_CARRAY(sfield);
	return q;
}


Query *QueryParser::MatchTerm(const UChar *field)
{
	// Func - matches for TERM
	//        TERM ::= TERM | PREFIXTERM | WILDTERM
	//            | NUMBER [<FUZZY>] [<CARAT> <NUMBER> [<FUZZY>]]
	// 			      | (<RANGEIN> | <RANGEEX>) [<CARAT> <NUMBER>]
	//            | <FULLFIELD>
	// 			      | <QUOTED> [SLOP] [<CARAT> <NUMBER>]
	// Pre  - field != NULL
	// Post -

	QueryToken *token = NULL;
	QueryToken *slop = NULL;
	QueryToken *boost = NULL;
	QueryToken *delToken = NULL; // Token that is about to be deleted

	bool prefix = false;
	bool wildcard = false;
	bool fuzzy = false;
	bool rangein = false;
	Query *q = NULL;
	QueryToken::Types type;

	token = mTokenList->Extract();

	switch (token->mType) {
	case QueryToken::TERM:
	case QueryToken::NUMBER:
	case QueryToken::PREFIXTERM:
	case QueryToken::WILDTERM:
		{
			// Check if type of QueryToken is a prefix term
			if (token->mType == QueryToken::PREFIXTERM) {
				prefix = true;
			}
			// Check if type of QueryToken is a wildcard term
			if (token->mType == QueryToken::WILDTERM) {
				wildcard = true;
			}
			// Peek to see if the type of the next token is fuzzy term
			type = mTokenList->Peek()->mType;
			if (type == QueryToken::FUZZY) {
				delToken = MatchQueryToken(QueryToken::FUZZY);

				CND_CONDITION(delToken != NULL, "DelToken is NULL");
				_CLDELETE(delToken);

				fuzzy = true;
			}
			type = mTokenList->Peek()->mType;
			if (type == QueryToken::CARAT) {
				delToken = MatchQueryToken(QueryToken::CARAT);

				CND_CONDITION(delToken != NULL, "DelToken is NULL");
				_CLDELETE(delToken);

				boost = MatchQueryToken(QueryToken::NUMBER);

				type = mTokenList->Peek()->mType;
				if (type == QueryToken::FUZZY) {
					delToken = MatchQueryToken(QueryToken::FUZZY);

					CND_CONDITION(delToken != NULL, "DelToken is NULL");
					_CLDELETE(delToken);

					fuzzy = true;
				}
			} // end if type==CARAT
			discardEscapeChar(token->mValue); // clean up
			if (wildcard) {
				q = GetWildcardQuery(field, token->mValue);
				break;
			}
			else if (prefix) {
				// Create a PrefixQuery
				token->mValue[_tcslen(token->mValue)-1] = 0; // Discard the *
				q = GetPrefixQuery(field, token->mValue);
				break;
			}
			else if (fuzzy) {
				// Create a FuzzyQuery
				// Remove the last char if it is a ~
				if (token->mValue[_tcslen(token->mValue)-1] == '~') {
					token->mValue[_tcslen(token->mValue)-1] = '\0';
				}
				q = GetFuzzyQuery(field, token->mValue);
				break;
			}
			else {
				q = GetFieldQuery(field, token->mValue);
				break;
			}
		}

	case QueryToken::RANGEIN:
	case QueryToken::RANGEEX:
		{
			if (token->mType == QueryToken::RANGEIN) {
				rangein = true;
			}

			type = mTokenList->Peek()->mType;
			if (type == QueryToken::CARAT) {
				delToken = MatchQueryToken(QueryToken::CARAT);

				CND_CONDITION(delToken != NULL, "DelToken is NULL");
				_CLDELETE(delToken);

				boost = MatchQueryToken(QueryToken::NUMBER);
			}

			UChar *noBrackets = token->mValue + 1;
			noBrackets[_tcslen(noBrackets)-1] = 0;
			q = ParseRangeQuery(field, noBrackets, rangein);
			break;
		}

	case QueryToken::FULLFIELD:
		{
			// Get the full field
			UChar *textField = token->mValue + 1;
			textField[_tcslen(textField)-1] = '\0';
			q = GetFullFieldQuery(field, textField);
			break;
		}

	case QueryToken::QUOTED:
		{
			type = mTokenList->Peek()->mType;
			if (type == QueryToken::SLOP) {
				slop = MatchQueryToken(QueryToken::SLOP);
			}
			type = mTokenList->Peek()->mType;
			if (type == QueryToken::CARAT) {
				delToken = MatchQueryToken(QueryToken::CARAT);
				CND_CONDITION(delToken != NULL, "DelToken is NULL");
				_CLDELETE(delToken);
				boost = MatchQueryToken(QueryToken::NUMBER);
			}
			// Remove the quotes
			UChar *quotedValue = token->mValue + 1;
			quotedValue[_tcslen(quotedValue)-1] = '\0';
			int32_t islop = phraseSlop;
			if (slop != NULL) {
				try {
					UChar *end; // todo: should parse using float...
					islop = (int32_t)_tcstoi64(slop->mValue + 1, &end, 10);
				}
				catch (...) {
					// ignored
				}
			}
			q = GetFieldQuery(field, quotedValue, islop);
			_CLDELETE(slop);
			break;
		}
	default:
		break;
	}

	_CLDELETE(token);


	if (q != NULL && boost != NULL) {
		float_t f = 1.0F;
		try {
			// UChar *tmp;
			// f = _tcstod(boost->mValue, &tmp);
			std::string value;
			UChar2String(value, boost->mValue);
			char *tmp;
			f = (float_t)std::strtod(value.c_str(), &tmp);
		}
		catch (...) {
			// ignored
		}
		_CLDELETE(boost);

		q->setBoost( f);
	}

	return q;
}

QueryToken *QueryParser::MatchQueryToken(QueryToken::Types expectedType)
{
	// Func - matches for QueryToken of the specified type and returns it otherwise Exception throws
	// Pre  - mTokenList != NULL
	// Post -

	CND_PRECONDITION(mTokenList != NULL, "Token list is NULL");
	CND_PRECONDITION(mTokenList->Count(), "Token list count is 0");

	// Extract a token form the TokenList
	QueryToken *token = mTokenList->Extract();
	// Check if the type of the token t matches the expectedType
	if (expectedType != token->mType) {
		gLog.log(eTypLogError, "Err > QueryParser: Unexpected QueryToken: %d, expected: %d", token->mType, expectedType);
		_CLDELETE(token);
	}

	// Return the matched token
	return token;
}

void QueryParser::ExtractAndDeleteToken(void)
{
	// Func - Extracts the first token from the Tokenlist tokenlist and destroys it
	// Pre  - true
	// Post - The first token has been extracted and destroyed

	CND_PRECONDITION(mTokenList != NULL, "Token list is NULL");

	// Extract the token from the TokenList
	QueryToken *token = mTokenList->Extract();
	// Condition Check Token may not be NULL
	CND_CONDITION(token != NULL, "Token is NULL");
	// Delete Token
	_CLDELETE(token);
}

CL_NS_END
