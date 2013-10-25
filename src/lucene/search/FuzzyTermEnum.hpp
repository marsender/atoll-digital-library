/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_FuzzyTermEnum_
#define _lucene_search_FuzzyTermEnum_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "FilteredTermEnum.hpp"
#include "FuzzyQuery.hpp"

namespace Atoll
{
class Searcher;
}

CL_NS_DEF(search)

/** FuzzyTermEnum is a subclass of FilteredTermEnum for enumerating all
*  terms that are similiar to the specified filter term.
*
*  Term enumerations are always ordered by Term.compareTo().  Each term in
*  the enumeration is greater than all that precede it.
*/
class FuzzyTermEnum: public FilteredTermEnum
{
private:
	float_t distance;
	bool _endEnum;

	CL_NS(index)::Term* searchTerm;
	UChar *text;
	size_t textLen;
	UChar *prefix;
	size_t prefixLength;
	float_t minimumSimilarity;
	double scale_factor;


	// This static array saves us from the time required to create a new array
	// everytime editDistance is called.
	int32_t* e;
	int32_t eWidth;
	int32_t eHeight;

	// Compute Levenshtein distance
	// Levenshtein distance also known as edit distance is a measure of similiarity
	// between two strings where the distance is measured as the number of character
	// deletions, insertions or substitutions required to transform one string to
	// the other string.
	// <p>This method takes in four parameters; two strings and their respective
	// lengths to compute the Levenshtein distance between the two strings.
	// The result is returned as an integer.
	int32_t editDistance(const UChar *s, const UChar *t, const int32_t n, const int32_t m) ;

protected:
	// The termCompare method in FuzzyTermEnum uses Levenshtein distance to
	// calculate the distance between the given term and the comparing term.
	bool termCompare(CL_NS(index)::Term* term) ;

	///Returns the fact if the current term in the enumeration has reached the end
	bool endEnum();
public:

	// Empty prefix and minSimilarity of 0.5f are used.
	FuzzyTermEnum(const Atoll::Searcher *inSearcher, CL_NS(index)::Term* term, float_t minSimilarity = FuzzyQuery::defaultMinSimilarity, size_t inPrefixLength = 0);
	//
	~FuzzyTermEnum();
	// Close the enumeration
	void close();

	// Returns the difference between the distance and the fuzzy threshold
	// multiplied by the scale factor
	float_t difference();

	const char* getObjectName() {
		return FuzzyTermEnum::getClassName();
	}
	static const char* getClassName() {
		return "FuzzyTermEnum";
	}
};

CL_NS_END
#endif
