/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_PrefixQuery
#define _lucene_search_PrefixQuery
#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Query.hpp"

CL_NS_DEF(index)
class Term;
class IndexReader;
CL_NS_END

CL_NS_DEF(search)
//! PrefixQuery is a Query that matches documents containing terms with a specified prefix.

class PrefixQuery: public Query
{
private:
	CL_NS(index)::Term* prefix;
protected:
	PrefixQuery(const PrefixQuery& clone);
public:

	//! Constructor. Constructs a query for terms starting with prefix
	PrefixQuery(CL_NS(index)::Term* Prefix);

	//! Destructor
	~PrefixQuery();

	//! Init string declarations
	static void InitStringDecl();

	//! Returns the name "PrefixQuery"
	const UChar *getQueryName() const;
	static const UChar *getClassName();

	/** Returns the prefix of this query. */
	CL_NS(index)::Term* getPrefix(bool pointer = true);

	Query* combine(Query** queries);
	Query* rewrite(Atoll::Searcher *inSearcher);
	Query* clone() const;
	bool equals(Query * other) const;

	//! Creates a user-readable version of this query and returns it as as string
	UChar *toString(const UChar *field) const;

	size_t hashCode() const;

	/**
	* Search the query and get the entry set result
	* @memory Caller must clean up
	*/
	Atoll::EntrySet* Search(Atoll::Searcher *inSearcher);
};

/*
class PrefixFilter: public Filter
{
private:
	CL_NS(index)::Term* prefix;
protected:
	PrefixFilter(const PrefixFilter& copy);

public:
	PrefixFilter(CL_NS(index)::Term* prefix);
	~PrefixFilter();

	//! Returns a BitSet with true for documents which should be permitted in
	//! search results, and false for those that should not.
	CL_NS(util)::BitSet* bits(CL_NS(index)::IndexReader* reader);

	Filter* clone() const;
	UChar *toString();
};
*/

CL_NS_END
#endif
