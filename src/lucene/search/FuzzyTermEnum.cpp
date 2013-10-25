/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "FuzzyTermEnum.hpp"
#include "../index/Term.hpp"
#include "../util/StringBuffer.hpp"
#include "../../common/Containers.hpp" // For CollatorStrncmp
#include "../../searcher/Searcher.hpp"
#include "unicode/ustring.h"
#include <limits> // For min / max

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)
using namespace Atoll;
using namespace Common;

/**
   * Constructor for enumeration of all terms from specified reader which share a prefix of
   * length prefixLength with term and which have a fuzzy similarity &gt; minSimilarity.
   *
   * @param inSearcher Searcher that delivers terms.
   * @param term Pattern term.
   * @param minSimilarity Minimum required similarity for terms from the reader. Default value is 0.5f.
   * @param inPrefixLength Length of required common prefix. Default value is 0.
   * @throws IOException
   */
FuzzyTermEnum::FuzzyTermEnum(const Searcher *inSearcher, Term* term, float_t minSimilarity, size_t inPrefixLength):
		distance(0),
		_endEnum(false),
		prefixLength(0),
		minimumSimilarity(minSimilarity)
{
	//Func - Constructor
	//Pre  - reader contains a valid reference to an IndexReader
	//       term != NULL
	//Post - The instance has been created

	CND_PRECONDITION(term != NULL, "term is NULL");

	scale_factor = 1.0f / (1.0f - minimumSimilarity);
	searchTerm = _CL_POINTER(term);

	text = STRDUP_TtoT(term->text());
	textLen = term->textLength();


	//Initialize e to NULL
	e          = NULL;
	eWidth     = 0;
	eHeight    = 0;

	if (inPrefixLength > 0 && inPrefixLength < textLen) {
		prefixLength = inPrefixLength;
		prefix = _CL_NEWARRAY(UChar, inPrefixLength + 1);
		_tcsncpy(prefix, text, inPrefixLength);
		prefix[inPrefixLength] = '\0';

		textLen = inPrefixLength;
		text[textLen] = '\0';
	}
	else {
		prefix = _CL_NEWARRAY(UChar, 1);
		prefix[0] = '\0';
	}

	//Set the enumeration
	Term* trm = _CLNEW Term(term->field(), prefix);
	setEnum(inSearcher->GetTermEnum(trm));
	_CLDECDELETE(trm);
}

FuzzyTermEnum::~FuzzyTermEnum()
{
	//Func - Destructor
	//Pre  - true
	//Post - FuzzyTermEnum has been destroyed

	//Close the enumeration
	close();
}

bool FuzzyTermEnum::endEnum()
{
	//Func - Returns the fact if the current term in the enumeration has reached the end
	//Pre  - true
	//Post - The boolean value of endEnum has been returned

	return _endEnum;
}

void FuzzyTermEnum::close()
{
	//Func - Close the enumeration
	//Pre  - true
	//Post - The enumeration has been closed

	FilteredTermEnum::close();

	//Finalize the searchTerm
	_CLDECDELETE(searchTerm);
	//Destroy e
	_CLDELETE_ARRAY(e);

	_CLDELETE_CARRAY(text);

	_CLDELETE_CARRAY(prefix);
}

bool FuzzyTermEnum::termCompare(Term* term)
{
	//Func - Compares term with the searchTerm using the Levenshtein distance.
	//Pre  - term is NULL or term points to a Term
	//Post - if pre(term) is NULL then false is returned otherwise
	//       if the distance of the current term in the enumeration is bigger than the FUZZY_THRESHOLD
	//       then true is returned

	if (term == NULL) {
		return false;  //Note that endEnum is not set to true!
	}

	const UChar *termText = term->text();
	size_t termTextLen = term->textLength();

	// Check if the field name of searchTerm of term match
	if (_tcscmp(searchTerm->field(), term->field()) == 0 &&
		(prefixLength == 0 || Common::CollatorStrncmp(termText, prefix, prefixLength) == 0)) {

		const UChar *target = termText + prefixLength;
		size_t targetLen = termTextLen - prefixLength;

		// Calculate the Levenshtein distance
		int32_t dist = editDistance(text, target, textLen, targetLen);
		distance = 1 - ((float_t)dist / (float_t)std::min(textLen, targetLen));
		return (distance > minimumSimilarity);
	}
	_endEnum = true;

	return false;
}

float_t FuzzyTermEnum::difference()
{
	//Func - Returns the difference between the distance and the fuzzy threshold
	//       multiplied by the scale factor
	//Pre  - true
	//Post - The difference is returned

	return (float_t)((distance - minimumSimilarity) * scale_factor);
}


// Finds and returns the smallest of three integers
// precondition: Must define int32_t __t for temporary storage and result
#define min3(a, b, c) __t = (a < b) ? a : b; __t = (__t < c) ? __t : c;

int32_t FuzzyTermEnum::editDistance(const UChar *s, const UChar *t, const int32_t n, const int32_t m)
{
	//Func - Calculates the Levenshtein distance also known as edit distance is a measure of similiarity
	//       between two strings where the distance is measured as the number of character
	//       deletions, insertions or substitutions required to transform one string to
	//       the other string.
	//Pre  - s != NULL and contains the source string
	//       t != NULL and contains the target string
	//       n >= 0 and contains the length of the source string
	//       m >= 0 and containts the length of th target string
	//Post - The distance has been returned

	CND_PRECONDITION(s != NULL, "s is NULL");
	CND_PRECONDITION(t != NULL, "t is NULL");
	CND_PRECONDITION(n >= 0, " n is a negative number");
	CND_PRECONDITION(n >= 0, " n is a negative number");

	int32_t i;     // iterates through s
	int32_t j;     // iterates through t
	UChar s_i; // ith character of s

	if (n == 0)
		return m;
	if (m == 0)
		return n;

	//Check if the array must be reallocated because it is too small or does not exist
	if (e == NULL || eWidth <= n || eHeight <= m) {
		//Delete e if possible
		_CLDELETE_ARRAY(e);
		//resize e
		eWidth  = std::max(eWidth, n + 1);
		eHeight = std::max(eHeight, m + 1);
		e = _CL_NEWARRAY(int32_t, eWidth * eHeight);
	}

	CND_CONDITION(e != NULL, "e is NULL");

	// init matrix e
	for (i = 0; i <= n; i++) {
		e[i + (0*eWidth)] = i;
	}
	for (j = 0; j <= m; j++) {
		e[0 + (j*eWidth)] = j;
	}

	int32_t __t; //temporary variable for min3

	// start computing edit distance
	for (i = 1; i <= n; i++) {
		s_i = s[i - 1];
		for (j = 1; j <= m; j++) {
			if (s_i != t[j-1]) {
				min3(e[i + (j*eWidth) - 1], e[i + ((j-1)*eWidth)], e[i + ((j-1)*eWidth)-1]);
				e[i + (j*eWidth)] = __t + 1;
			}
			else {
				min3(e[i + (j*eWidth) -1] + 1, e[i + ((j-1)*eWidth)] + 1, e[i + ((j-1)*eWidth)-1]);
				e[i + (j*eWidth)] = __t;
			}
		}
	}

	// we got the result!
	return e[n + ((m)*eWidth)];
}

CL_NS_END
