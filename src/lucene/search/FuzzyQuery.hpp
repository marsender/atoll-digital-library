/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_FuzzyQuery_
#define _lucene_search_FuzzyQuery_

#include "MultiTermQuery.hpp"

CL_NS_DEF(search)

// class FuzzyQuery implements the fuzzy search query
class FuzzyQuery: public MultiTermQuery
{
private:
	float_t minimumSimilarity;
	size_t prefixLength;
protected:
	FuzzyQuery(const FuzzyQuery& clone);
public:
	static float_t defaultMinSimilarity;

	/**
	* Create a new FuzzyQuery that will match terms with a similarity
	* of at least <code>minimumSimilarity</code> to <code>term</code>.
	* If a <code>prefixLength</code> &gt; 0 is specified, a common prefix
	* of that length is also required.
	*
	* @param term the term to search for
	* @param minimumSimilarity a value between 0 and 1 to set the required similarity
	*  between the query term and the matching terms. For example, for a
	*  <code>minimumSimilarity</code> of <code>0.5</code> a term of the same length
	*  as the query term is considered similar to the query term if the edit distance
	*  between both terms is less than <code>length(term)*0.5</code>
	* @param prefixLength length of common (non-fuzzy) prefix
	* @throws IllegalArgumentException if minimumSimilarity is &gt; 1 or &lt; 0
	* or if prefixLength &lt; 0 or &gt; <code>term.text().length()</code>.
	*/
	FuzzyQuery(CL_NS(index)::Term* term, float_t minimumSimilarity = defaultMinSimilarity, size_t prefixLength = 0);
	//Destructor
	~FuzzyQuery();

	UChar *toString(const UChar *field) const;

	//! Init string declarations
	static void InitStringDecl();

	//Returns the name "FuzzyQuery"
	static const UChar *getClassName();
	const UChar *getQueryName() const;

	Query* clone() const;
	bool equals(Query * other) const;
	size_t hashCode() const;

	/**
	* Returns the minimum similarity that is required for this query to match.
	* @return float value between 0.0 and 1.0
	*/
	float_t getMinSimilarity() const;

	/**
	* Returns the prefix length, i.e. the number of characters at the start
	* of a term that must be identical (not fuzzy) to the query term if the query
	* is to match that term.
	*/
	size_t getPrefixLength() const;

protected:
	FilteredTermEnum* getEnum(Atoll::Searcher *inSearcher);
};

CL_NS_END
#endif
