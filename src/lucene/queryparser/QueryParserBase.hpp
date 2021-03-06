/*******************************************************************************

QueryParserBase.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __QueryParserBase_HPP
#define __QueryParserBase_HPP
//------------------------------------------------------------------------------

#include <vector>
//------------------------------------------------------------------------------

CL_NS_DEF(analysis)
class Analyzer;
CL_NS_END

CL_NS_DEF(search)
class BooleanClause;
class Query;
CL_NS_END

CL_NS_DEF(queryParser)

/**
* Contains default implementations used by QueryParser.
* You can override any of these to provide a customised QueryParser.
*/
class QueryParserBase
{
protected:
	/* The actual operator the parser uses to combine query terms */
	int defaultOperator;
	int32_t phraseSlop;

	bool lowercaseExpandedTerms;

	LUCENE_STATIC_CONSTANT(int,  CONJ_NONE = 0);
	LUCENE_STATIC_CONSTANT(int,  CONJ_AND = 1);
	LUCENE_STATIC_CONSTANT(int,  CONJ_OR = 2);

	LUCENE_STATIC_CONSTANT(int,  MOD_NONE = 0);
	LUCENE_STATIC_CONSTANT(int,  MOD_NOT = 10);
	LUCENE_STATIC_CONSTANT(int,  MOD_REQ = 11);

	CL_NS(analysis)::Analyzer* analyzer;

public:
	QueryParserBase(CL_NS(analysis)::Analyzer* analyzer);
	virtual ~QueryParserBase();

	//! Init string declarations
	static void InitStringDecl();

	/**
	* Whether terms of wildcard, prefix, fuzzy and range queries are to be automatically
	* lower-cased or not.  Default is <code>true</code>.
	*/
	void  setLowercaseExpandedTerms(bool lowercaseExpandedTerms);

	/*
	* @see #setLowercaseExpandedTerms(boolean)
	*/
	//bool getLowercaseExpandedTerms() const;

	//values used for setOperator
	LUCENE_STATIC_CONSTANT(int,  OR_OPERATOR = 0);
	LUCENE_STATIC_CONSTANT(int,  AND_OPERATOR = 1);

	/**
	* Sets the boolean operator of the QueryParser.
	* In default mode (<code>OR_OPERATOR</code>) terms without any modifiers
	* are considered optional: for example <code>capital of Hungary</code> is equal to
	* <code>capital OR of OR Hungary</code>.<br/>
	* In <code>AND_OPERATOR</code> mode terms are considered to be in conjuction: the
	* above mentioned query is parsed as <code>capital AND of AND Hungary</code>
	*/
	void setDefaultOperator(int oper);
	/**
	* Gets implicit operator setting, which will be either AND_OPERATOR
	* or OR_OPERATOR.
	*/
	//int getDefaultOperator() const;

	//public so that the lexer can call this
	//virtual void throwParserException(const UChar *message, UChar ch, int32_t col, int32_t line);

	/**
	* Sets the default slop for phrases.  If zero, then exact phrase matches
	* are required.  Default value is zero.
	*/
	void setPhraseSlop(int phraseSlop) {
		this->phraseSlop = phraseSlop;
	}

	/**
	* Gets the default slop for phrases.
	*/
	int getPhraseSlop() const {
		return phraseSlop;
	}

protected:

	/**
	* Removes the escaped characters
	*/
	void discardEscapeChar(UChar *token) const;

	//Analyzes the expanded term termStr with the StandardFilter and the LowerCaseFilter.
	UChar *AnalyzeExpandedTerm(const UChar *field, UChar *termStr);

	// Adds the next parsed clause.
	virtual void AddClause(std::vector < CL_NS(search)::BooleanClause* > & clauses, int32_t conj, int32_t mods, CL_NS(search)::Query* q);

	/**
	* Returns a termquery for the specified full field or NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetFullFieldQuery(const UChar *field, UChar *queryText);

	/**
	* Returns a termquery, phrasequery for the specified field or NULL to disallow
	* Note: this is only a partial implementation, since MultiPhraseQuery is not implemented yet
	*/
	virtual CL_NS(search)::Query* GetFieldQuery(const UChar *field, UChar *queryText);

	/**
	* Delegates to GetFieldQuery(string, string), and adds slop onto phrasequery.
	* Can be used to remove slop functionality
	*/
	virtual CL_NS(search)::Query* GetFieldQuery(const UChar *field, UChar *queryText, int32_t slop);

	/**
	* Factory method for generating a query (similar to
	* {@link #GetWildcardQuery}). Called when parser parses an input term
	* token that uses prefix notation; that is, contains a single '*' wildcard
	* character as its last character. Since this is a special case
	* of generic wildcard term, and such a query can be optimized easily,
	* this usually results in a different query object.
	*<p>
	* Depending on settings, a prefix term may be lower-cased
	* automatically. It will not go through the default Analyzer,
	* however, since normal Analyzers are unlikely to work properly
	* with wildcard templates.
	*<p>
	* Can be overridden by extending classes, to provide custom handling for
	* wild card queries, which may be necessary due to missing analyzer calls.
	*
	* @param field Name of the field query will use.
	* @param termStr Term token to use for building term for the query
	*    (<b>without</b> trailing '*' character!)
	*
	* @return Resulting query built for the term or NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetPrefixQuery(const UChar *field, UChar *termStr);

	/**
	* Factory method for generating a query. Called when parser
	* parses an input term token that contains one or more wildcard
	* characters (? and *), but is not a prefix term token (one
	* that has just a single * character at the end)
	*<p>
	* Depending on settings, prefix term may be lower-cased
	* automatically. It will not go through the default Analyzer,
	* however, since normal Analyzers are unlikely to work properly
	* with wildcard templates.
	*<p>
	* Can be overridden by extending classes, to provide custom handling for
	* wildcard queries, which may be necessary due to missing analyzer calls.
	*
	* @param field Name of the field query will use.
	* @param termStr Term token that contains one or more wild card
	*   characters (? or *), but is not simple prefix term
	*
	* @return Resulting query built for the term or NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetWildcardQuery(const UChar *field, UChar *termStr);

	/**
	* Factory method for generating a query (similar to
	* {@link #GetWildcardQuery}). Called when parser parses
	* an input term token that has the fuzzy suffix (~) appended.
	*
	* @param field Name of the field query will use.
	* @param termStr Term token to use for building term for the query
	*
	* @return Resulting query built for the term or NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetFuzzyQuery(const UChar *field, UChar *termStr);

	/**
	* Factory method for generating query, given a set of clauses.
	* By default creates a boolean query composed of clauses passed in.
	*
	* Can be overridden by extending classes, to modify query being
	* returned.
	*
	* Memory: clauses must all be cleaned up by this function.
	*
	* @param clauses Vector that contains BooleanClause instances to join.
	*
	* @return Resulting query object or NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetBooleanQuery(std::vector < CL_NS(search)::BooleanClause* > & clauses);

	/**
	* return NULL to disallow
	*/
	virtual CL_NS(search)::Query* GetRangeQuery(const UChar *field, UChar *part1, UChar *part2, bool inclusive);
	virtual CL_NS(search)::Query* ParseRangeQuery(const UChar *field, UChar *str, bool inclusive);

};
//------------------------------------------------------------------------------

CL_NS_END

//------------------------------------------------------------------------------
#endif
