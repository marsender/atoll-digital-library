/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_RangeQuery_
#define _lucene_search_RangeQuery_
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Query.hpp"
//#include "Scorer.hpp"
//#include "TermQuery.hpp"
//#include "../index/Term.hpp"
//#include "../index/Terms.hpp"
//#include "../util/StringBuffer.hpp"

CL_NS_DEF(index)
class Term;
class IndexReader;
CL_NS_END

CL_NS_DEF(search)
/** Constructs a query selecting all terms greater than
 * <code>lowerTerm</code> but less than <code>upperTerm</code>.
 * There must be at least one term and either term may be null,
 * in which case there is no bound on that side, but if there are
 * two terms, both terms <b>must</b> be for the same field.
 */
class RangeQuery: public Query
{
private:
	CL_NS(index)::Term* lowerTerm;
	CL_NS(index)::Term* upperTerm;
	bool inclusive;
protected:
	RangeQuery(const RangeQuery& clone);

public:
	// Constructs a query selecting all terms greater than
	// <code>lowerTerm</code> but less than <code>upperTerm</code>.
	// There must be at least one term and either term may be NULL--
	// in which case there is no bound on that side, but if there are
	// two term, both terms <b>must</b> be for the same field.
	RangeQuery(CL_NS(index)::Term* LowerTerm, CL_NS(index)::Term* UpperTerm, const bool Inclusive);
	~RangeQuery();

	//! Init string declarations
	static void InitStringDecl();

	const UChar *getQueryName() const;
	static const UChar *getClassName();

	Query* rewrite(Atoll::Searcher *inSearcher);

	Query* combine(Query** queries);

	//! Prints a user-readable version of this query
	UChar *toString(const UChar *field) const;

	Query* clone() const;

	bool equals(Query * other) const;

	CL_NS(index)::Term* getLowerTerm(bool pointer = true) const;
	CL_NS(index)::Term* getUpperTerm(bool pointer = true) const;
	bool isInclusive() const;
	const UChar *getField() const;

	size_t hashCode() const;

	/**
	* Search the query and get the entry set result
	* @memory Caller must clean up
	*/
	Atoll::EntrySet* Search(Atoll::Searcher *inSearcher);
};

CL_NS_END
#endif
