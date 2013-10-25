/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "WildcardTermEnum.hpp"
#include "../index/Term.hpp"
//#include "../index/Terms.hpp"
#include "../../common/Containers.hpp" // For CollatorStrncmp
#include "../../searcher/Searcher.hpp"
#include "unicode/ustring.h"
#include <limits> // For min / max

CL_NS_USE(index)
CL_NS_DEF(search)
using namespace Atoll;

bool WildcardTermEnum::termCompare(Term* term)
{
	if (term != NULL && _tcscmp(__term->field(), term->field()) == 0) {
		const UChar *searchText = term->text();
		const UChar *patternText = __term->text();
		if (Common::CollatorStrncmp(searchText, pre, preLen) == 0) {
			return wildcardEquals(patternText + preLen, __term->textLength() - preLen, 0, searchText, term->textLength(), preLen);
		}
	}
	_endEnum = true;
	return false;
}

WildcardTermEnum::WildcardTermEnum(Searcher *inSearcher, Term* term):
		FilteredTermEnum(),
		__term(_CL_POINTER(term)),
		fieldMatch(false),
		_endEnum(false)
{

	pre = stringDuplicate(term->text());

	const UChar *sidx = _tcschr(pre, LUCENE_WILDCARDTERMENUM_WILDCARD_STRING);
	const UChar *cidx = _tcschr(pre, LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR);
	const UChar *tidx = sidx;
	if (tidx == NULL)
		tidx = cidx;
	else if (cidx && cidx > pre)
		tidx = std::min(sidx, cidx);
	CND_PRECONDITION(tidx != NULL, "tidx==NULL");
	int32_t idx = (int32_t)(tidx - pre);
	preLen = idx;
	CND_PRECONDITION(preLen < (int32_t)term->textLength(), "preLen >= term->textLength()");
	pre[preLen] = 0; //trim end

	Term* t = _CLNEW Term(__term->field(), pre);
	setEnum(inSearcher->GetTermEnum(t));
	_CLDECDELETE(t);
}

WildcardTermEnum::~WildcardTermEnum()
{
	close();
}

void WildcardTermEnum::close()
{
	if (__term != NULL) {
		FilteredTermEnum::close();

		_CLDECDELETE(__term);
		__term = NULL;

		_CLDELETE_CARRAY(pre);
	}
}

float_t WildcardTermEnum::difference()
{
	return 1.0f;
}

bool WildcardTermEnum::endEnum()
{
	return _endEnum;
}

bool WildcardTermEnum::wildcardEquals(const UChar *pattern, int32_t patternLen, int32_t patternIdx, const UChar *str, int32_t strLen, int32_t stringIdx)
{
	for (int32_t p = patternIdx; ; ++p) {
		for (int32_t s = stringIdx; ; ++p, ++s) {
			// End of str yet?
			bool sEnd = (s >= strLen);
			// End of pattern yet?
			bool pEnd = (p >= patternLen);

			// If we're looking at the end of the str...
			if (sEnd) {
				// Assume the only thing left on the pattern is/are wildcards
				bool justWildcardsLeft = true;

				// Current wildcard position
				int32_t wildcardSearchPos = p;
				// While we haven't found the end of the pattern,
				// and haven't encountered any non-wildcard characters
				while (wildcardSearchPos < patternLen && justWildcardsLeft) {
					// Check the character at the current position
					UChar wildchar = pattern[wildcardSearchPos];
					// If it's not a wildcard character, then there is more
					// pattern information after this/these wildcards.

					if (wildchar != LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR &&
					    wildchar != LUCENE_WILDCARDTERMENUM_WILDCARD_STRING) {
						justWildcardsLeft = false;
					}
					else {
						// to prevent "cat" matches "ca??"
						if (wildchar == LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR)
							return false;

						wildcardSearchPos++; // Look at the next character
					}
				}

				// This was a prefix wildcard search, and we've matched, so
				// return true.
				if (justWildcardsLeft)
					return true;
			}

			// If we've gone past the end of the str, or the pattern,
			// return false.
			if (sEnd || pEnd)
				break;

			// Match a single character, so continue.
			if (pattern[p] == LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR)
				continue;

			if (pattern[p] == LUCENE_WILDCARDTERMENUM_WILDCARD_STRING) {
				// Look at the character beyond the '*'.
				++p;
				// Examine the str, starting at the last character.
				for (int32_t i = strLen; i >= s; --i) {
					if (wildcardEquals(pattern, patternLen, p, str, strLen, i))
						return true;
				}
				break;
			}
			if (pattern[p] != str[s])
				break;
		}
		return false;
	}
}

CL_NS_END
