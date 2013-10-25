/*******************************************************************************

QueryParserBase.cpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#include "../Lucene.hpp"
#include "QueryParserBase.hpp"
#include "../analysis/Analyzers.hpp"
#include "../search/BooleanQuery.hpp"
#include "../search/FuzzyQuery.hpp"
#include "../search/PhraseQuery.hpp"
#include "../search/RangeQuery.hpp"
#include "../search/PrefixQuery.hpp"
#include "../search/TermQuery.hpp"
#include "../search/WildcardQuery.hpp"
#include "../index/Term.hpp"
//#include "unicode/ustdio.h"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

static const UChar cStrFieldPattern[] = { 0x00B7, 'f', 0x00B7, 0 }; // ·f·
static const UChar cStrWordPattern[] = { 0x00B7, 'w', 0x00B7, 0 }; // ·w·

U_STRING_DECL(cStrTo, "TO", 2);
CL_NS_DEF(queryParser)
void QueryParserBase::InitStringDecl() {
	U_STRING_INIT(cStrTo, "TO", 2);
}
CL_NS_END

CL_NS_USE(search)
CL_NS_USE(util)
CL_NS_USE(analysis)
CL_NS_USE(index)
using namespace Common;

CL_NS_DEF(queryParser)

QueryParserBase::QueryParserBase(Analyzer* analyzer)
{
//Func - Constructor
//Pre  - true
//Post - instance has been created with PhraseSlop = 0
	this->analyzer = analyzer;
	this->defaultOperator = OR_OPERATOR;
	this->phraseSlop = 0;
	this->lowercaseExpandedTerms = true;
}

QueryParserBase::~QueryParserBase()
{
//Func - Destructor
//Pre  - true
//Post - The instance has been destroyed
}


void QueryParserBase::discardEscapeChar(UChar *source) const
{
	int len = _tcslen(source);
	for (int i = 0; i < len; i++) {
		if (source[i] == '\\' && source[i+1] != '\0') {
			_tcscpy(source + i, source + i + 1);
			len--;
		}
	}
}

void QueryParserBase::AddClause(std::vector<BooleanClause*>& clauses, int32_t conj, int32_t mods, Query* q)
{
//Func - Adds the next parsed clause.
//Pre  -
//Post -

	bool required, prohibited;

	// If this term is introduced by AND, make the preceding term required,
	// unless it's already prohibited.
	const uint32_t nPreviousClauses = clauses.size();
	if (nPreviousClauses > 0 && conj == CONJ_AND) {
		BooleanClause* c = clauses[nPreviousClauses-1];
		if (!c->prohibited)
			c->required = true;
	}

	if (nPreviousClauses > 0 && defaultOperator == AND_OPERATOR && conj == CONJ_OR) {
		// If this term is introduced by OR, make the preceding term optional,
		// unless it's prohibited (that means we leave -a OR b but +a OR b-->a OR b)
		// notice if the input is a OR b, first term is parsed as required; without
		// this modification a OR b would parse as +a OR b
		BooleanClause* c = clauses[nPreviousClauses-1];
		if (!c->prohibited) {
			c->required = false;
			c->prohibited = false;
		}
	}

	// We might have been passed a NULL query; the term might have been
	// filtered away by the analyzer.
	if (q == NULL)
		return;

	if (defaultOperator == OR_OPERATOR) {
		// We set REQUIRED if we're introduced by AND or +; PROHIBITED if
		// introduced by NOT or -; make sure not to set both.
		prohibited = (mods == MOD_NOT);
		required = (mods == MOD_REQ);
		if (conj == CONJ_AND && !prohibited) {
			required = true;
		}
	}
	else {
		// We set PROHIBITED if we're introduced by NOT or -; We set REQUIRED
		// if not PROHIBITED and not introduced by OR
		prohibited = (mods == MOD_NOT);
		required = (!prohibited && conj != CONJ_OR);
	}

	if (required && prohibited)
		gLog.log(eTypLogError, "Err > QueryParserBase: Clause cannot be both required and prohibited");

	clauses.push_back(_CLNEW BooleanClause(q, true, required, prohibited));
}

/*
void QueryParserBase::throwParserException(const UChar *message, UChar ch, int32_t col, int32_t line)
{
	UChar msg[1024];
	_sntprintf(msg, 1024, message, ch, col, line);
	_CLTHROWT (CL_ERR_Parse, msg);
}
*/

Query* QueryParserBase::GetFullFieldQuery(const UChar *field, UChar *queryText)
{
	// Set the field pattern
	UChar *p = _tcsstr(field, cStrWordPattern);
	if (p) {
		*(p + 1) = *(cStrFieldPattern + 1);
	}

	Term* t = _CLNEW Term(field, queryText);
	Query* ret = _CLNEW TermQuery(t);
	_CLDECDELETE(t);

	// Reset the field pattern
	if (p) {
		*(p + 1) = *(cStrWordPattern + 1);
	}

	return ret;
}

Query* QueryParserBase::GetFieldQuery(const UChar *field, UChar *queryText)
{
//Func - Returns a query for the specified field.
//       Use the analyzer to get all the tokens, and then build a TermQuery,
//       PhraseQuery, or nothing based on the term count
//Pre  - field != NULL
//       analyzer contains a valid reference to an Analyzer
//       queryText != NULL and contains the query
//Post - A query instance has been returned for the specified field

	CND_PRECONDITION(field != NULL, "field is NULL");
	CND_PRECONDITION(queryText != NULL, "queryText is NULL");

	//Instantiate a stringReader for queryText
	StringReader reader(queryText);
	TokenStream* source = analyzer->tokenStream(field, &reader);
	CND_CONDITION(source != NULL, "source is NULL");

	StringArrayConstWithDeletor v;

	Token t;
	int positionCount = 0;
	bool severalTokensAtSamePosition = false;

	// Get the tokens from the source
	try {
		while (source->next(&t)) {
			v.push_back(STRDUP_TtoT(t.termText()));

			if (t.getPositionIncrement() != 0)
				positionCount += t.getPositionIncrement();
			else
				severalTokensAtSamePosition = true;
		}
	}
	catch (CLuceneError& err) {
		if (err.getExceptionCode() != CL_ERR_IO)
			throw err;
	}
	_CLDELETE(source);

	// Check if there are any tokens retrieved
	if (v.size() == 0) {
		return NULL;
	}
	else {
		if (v.size() == 1) {
			Term* t = _CLNEW Term(field, v[0]);
			Query* ret = _CLNEW TermQuery(t);
			_CLDECDELETE(t);
			return ret;
		}
		else {
			if (severalTokensAtSamePosition) {
				if (positionCount == 1) {
					// no phrase query:
					BooleanQuery* q = _CLNEW BooleanQuery; //todo: disableCoord=true here, but not implemented in BooleanQuery
					StringArrayConst::iterator itr = v.begin();
					while (itr != v.end()) {
						Term* t = _CLNEW Term(field, *itr);
						q->add(_CLNEW TermQuery(t), true, false, false);//should occur...
						_CLDECDELETE(t);
						++itr;
					}
					return q;
				}
				else {
					_CLTHROWA(CL_ERR_UnsupportedOperation, "MultiPhraseQuery NOT Implemented");
				}
			}
			else {
				PhraseQuery* q = _CLNEW PhraseQuery;
				q->setSlop(phraseSlop);

				StringArrayConst::iterator itr = v.begin();
				while (itr != v.end()) {
					const UChar *data = *itr;
					Term* t = _CLNEW Term(field, data);
					q->add(t);
					_CLDECDELETE(t);
					++itr;
				}
				return q;
			}
		}
	}
}

Query* QueryParserBase::GetFieldQuery(const UChar *field, UChar *queryText, int32_t slop)
{
	Query* ret = GetFieldQuery(field, queryText);
	if (ret && ret->getQueryName() == PhraseQuery::getClassName()) {
		PhraseQuery *phraseQuery = static_cast<PhraseQuery *>(ret);
		phraseQuery->setSlop(slop);
	}

	return ret;
}

void QueryParserBase::setLowercaseExpandedTerms(bool lowercaseExpandedTerms)
{
	this->lowercaseExpandedTerms = lowercaseExpandedTerms;
}

/*
bool QueryParserBase::getLowercaseExpandedTerms() const
{
	return lowercaseExpandedTerms;
}
*/

void QueryParserBase::setDefaultOperator(int oper)
{
	this->defaultOperator = oper;
}

/*
int QueryParserBase::getDefaultOperator() const
{
	return defaultOperator;
}
*/

Query* QueryParserBase::ParseRangeQuery(const UChar *field, UChar *queryText, bool inclusive)
{
	//todo: this must be fixed, [-1--5] (-1 to -5) should yield a result, but won't parse properly
	//because it uses an analyser, should split it up differently...

	// Use the analyzer to get all the tokens.  There should be 1 or 2.
	StringReader reader(queryText);
	TokenStream* source = analyzer->tokenStream(field, &reader);

	UChar *terms[2];
	terms[0] = NULL;
	terms[1] = NULL;
	Token t;
	bool tret = true;
	bool from = true;
	while (tret) {
		try {
			tret = source->next(&t);
		}
		catch (CLuceneError& err) {
			if (err.getExceptionCode() == CL_ERR_IO)
				tret = false;
			else
				throw err;
		}
		if (tret) {
			if (!from && _tcscmp(t.termText(), cStrTo) == 0)
				continue;
			UChar *tmp = STRDUP_TtoT(t.termText());
			discardEscapeChar(tmp);
			terms[from? 0 : 1] = tmp;
			if (from)
				from = false;
			else
				break;
		}
	}
	Query* ret = GetRangeQuery(field, terms[0], terms[1], inclusive);
	_CLDELETE_CARRAY(terms[0]);
	_CLDELETE_CARRAY(terms[1]);
	_CLDELETE(source);

	return ret;
}

Query* QueryParserBase::GetBooleanQuery(std::vector < CL_NS(search)::BooleanClause* > & clauses)
{
	if (clauses.empty())
		return NULL;

	BooleanQuery* query = _CLNEW BooleanQuery();
	//Condition check to see if query has been allocated properly
	CND_CONDITION(query != NULL, "No memory could be allocated for query");

	//iterate through all the clauses
	const uint32_t count = clauses.size();
	for (uint32_t i = 0; i < count; i++) {
		//Condition check to see if clauses[i] is valdid
		CND_CONDITION(clauses[i] != NULL, "clauses[i] is NULL");
		//Add it to query
		query->add(clauses[i]);
	}
	return query;
}

Query* QueryParserBase::GetPrefixQuery(const UChar *field, UChar *termStr)
{
//Pre  - field != NULL and field contains the name of the field that the query will use
//       termStr != NULL and is the  token to use for building term for the query
//       (WITH or WITHOUT a trailing '*' character!)
//Post - A PrefixQuery instance has been returned

	CND_PRECONDITION(field != NULL, "field is NULL");
	CND_PRECONDITION(termStr != NULL, "termStr is NULL");

	if (lowercaseExpandedTerms)
		_tcslwr(termStr);

	Term* t = _CLNEW Term(field, termStr);
	CND_CONDITION(t != NULL, "Could not allocate memory for term t");

	Query *q = _CLNEW PrefixQuery(t);
	CND_CONDITION(q != NULL, "Could not allocate memory for PrefixQuery q");

	_CLDECDELETE(t);
	return q;
}

Query* QueryParserBase::GetFuzzyQuery(const UChar *field, UChar *termStr)
{
//Func - Factory method for generating a query (similar to getPrefixQuery}). Called when parser parses
//       an input term token that has the fuzzy suffix (~) appended.
//Pre  - field != NULL and field contains the name of the field that the query will use
//       termStr != NULL and is the  token to use for building term for the query
//       (WITH or WITHOUT a trailing '*' character!)
//Post - A FuzzyQuery instance has been returned

	CND_PRECONDITION(field != NULL, "field is NULL");
	CND_PRECONDITION(termStr != NULL, "termStr is NULL");

	if (lowercaseExpandedTerms)
		_tcslwr(termStr);

	Term* t = _CLNEW Term(field, termStr);
	CND_CONDITION(t != NULL, "Could not allocate memory for term t");

	Query *q = _CLNEW FuzzyQuery(t);
	CND_CONDITION(q != NULL, "Could not allocate memory for FuzzyQuery q");

	_CLDECDELETE(t);
	return q;
}

Query* QueryParserBase::GetWildcardQuery(const UChar *field, UChar *termStr)
{
	CND_PRECONDITION(field != NULL, "field is NULL");
	CND_PRECONDITION(termStr != NULL, "termStr is NULL");

	if (lowercaseExpandedTerms)
		_tcslwr(termStr);

	Term* t = _CLNEW Term(field, termStr);
	CND_CONDITION(t != NULL, "Could not allocate memory for term t");
	Query* q = _CLNEW WildcardQuery(t);
	_CLDECDELETE(t);

	return q;
}

CL_NS(search)::Query* QueryParserBase::GetRangeQuery(const UChar *field, UChar *part1, UChar *part2, bool inclusive)
{
	//todo: does jlucene handle rangequeries differntly? if we are using
	//a certain type of analyser, the terms may be filtered out, which
	//is not necessarily what we want.
	if (lowercaseExpandedTerms) {
		_tcslwr(part1);
		_tcslwr(part2);
	}
	//todo: should see if we can parse the strings as dates... currently we leave that up to the end-developer...
	Term* t1 = _CLNEW Term(field, part1);
	Term* t2 = _CLNEW Term(field, part2);
	Query* ret = _CLNEW RangeQuery(t1, t2, inclusive);
	_CLDECDELETE(t1);
	_CLDECDELETE(t2);

	return ret;
}

CL_NS_END
