/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_WildcardQuery_
#define _lucene_search_WildcardQuery_

#include "MultiTermQuery.hpp"

CL_NS_DEF(index)
class IndexReader;
CL_NS_END

CL_NS_DEF(search)

/** Implements the wildcard search query. Supported wildcards are <code>*</code>, which
 * matches any character sequence (including the empty one), and <code>?</code>,
 * which matches any single character. Note this query can be slow, as it
 * needs to iterate over all terms. In order to prevent extremely slow WildcardQueries,
 * a Wildcard term must not start with one of the wildcards <code>*</code> or
 * <code>?</code>.
 *
 * @see WildcardTermEnum
 */
class WildcardQuery: public MultiTermQuery
{
protected:
	FilteredTermEnum* getEnum(Atoll::Searcher *inSearcher);
	WildcardQuery(const WildcardQuery& clone);
public:
	WildcardQuery(CL_NS(index)::Term* term);
	~WildcardQuery();

	//! Init string declarations
	static void InitStringDecl();

	//Returns the string "WildcardQuery"
	const UChar *getQueryName() const;
	static const UChar *getClassName();

	size_t hashCode() const;
	bool equals(Query *other) const;
	Query* clone() const;
};

/*
class WildcardFilter: public Filter
{
private:
	CL_NS(index)::Term* term;
protected:
	WildcardFilter(const WildcardFilter& copy);

public:
	WildcardFilter(CL_NS(index)::Term* term);
	~WildcardFilter();

	// Returns a BitSet with true for documents which should be permitted in
	// search results, and false for those that should not.
	CL_NS(util)::BitSet* bits(CL_NS(index)::IndexReader* reader);

	Filter* clone() const;
	UChar *toString();
};
*/


CL_NS_END
#endif
