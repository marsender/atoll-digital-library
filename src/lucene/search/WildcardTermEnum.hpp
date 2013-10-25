/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_WildcardTermEnum_
#define _lucene_search_WildcardTermEnum_

#include "FilteredTermEnum.hpp"

namespace Atoll
{
class Searcher;
}

CL_NS_DEF(index)
class Term;
class IndexReader;
CL_NS_END

CL_NS_DEF(search)
/**
 * Subclass of FilteredTermEnum for enumerating all terms that match the
 * specified wildcard filter term.
 * Term enumerations are always ordered by term->compareTo().
 * Each term in the enumeration is greater than all that precede it.
 */
class WildcardTermEnum: public FilteredTermEnum
{
private:
	CL_NS(index)::Term* __term;
	UChar *pre;
	int32_t preLen;
	bool fieldMatch;
	bool _endEnum;

	// const UChar *equality with support for wildcards

protected:
	bool termCompare(CL_NS(index)::Term* term) ;

public:

	/**
	 * Creates a new <code>WildcardTermEnum</code>.
	 * Passing in a term that does not contain a
	 * <code>LUCENE_WILDCARDTERMENUM_WILDCARD_STRING</code> or
	 * <code>LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR</code> will cause an exception to be thrown.
	 */
	WildcardTermEnum(Atoll::Searcher *inSearcher, CL_NS(index)::Term* term);
	~WildcardTermEnum();

	float_t difference() ;

	bool endEnum() ;

	/**
	 * Determines if a word matches a wildcard pattern.
	 */
	static bool wildcardEquals(const UChar *pattern, int32_t patternLen, int32_t patternIdx, const UChar *str, int32_t strLen, int32_t stringIdx);

	void close();

	const char* getObjectName() {
		return WildcardTermEnum::getClassName();
	}
	static const char* getClassName() {
		return "WildcardTermEnum";
	}
};
CL_NS_END
#endif
