/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "FuzzyQuery.hpp"
#include "FuzzyTermEnum.hpp"
#include "../index/Term.hpp"
#include "../util/StringBuffer.hpp"
//#include "../index/IndexReader.hpp"
//#include "../../searcher/Searcher.hpp"
#include "unicode/ustring.h"

U_STRING_DECL(cStrClassName, "FuzzyQuery", 10);
CL_NS_DEF(search)
void FuzzyQuery::InitStringDecl() {
	U_STRING_INIT(cStrClassName, "FuzzyQuery", 10);
}
CL_NS_END

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)
using namespace Atoll;
using namespace Common;

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
FuzzyQuery::FuzzyQuery(Term* term, float_t minimumSimilarity, size_t prefixLength):
		MultiTermQuery(term)
{
	//Func - Constructor
	//Pre  - term != NULL
	//Post - The instance has been created

	CND_PRECONDITION(term != NULL, "term is NULL");

	if (minimumSimilarity > 1.0f)
		_CLTHROWA(CL_ERR_IllegalArgument, "minimumSimilarity > 1");
	else if (minimumSimilarity < 0.0f)
		_CLTHROWA(CL_ERR_IllegalArgument, "minimumSimilarity < 0");

	this->minimumSimilarity = minimumSimilarity;

	if (prefixLength >= term->textLength())
		_CLTHROWA(CL_ERR_IllegalArgument, "prefixLength >= term.textLength()");
	this->prefixLength = prefixLength;
}

float_t FuzzyQuery::defaultMinSimilarity = 0.5f;

FuzzyQuery::~FuzzyQuery()
{
	//Func - Destructor
	//Pre  - true
	//Post - Instance has been destroyed
}

UChar *FuzzyQuery::toString(const UChar *field) const
{
	//Func - Returns the query string
	//Pre  - field != NULL
	//Post - The query string has been returned

	CND_PRECONDITION(field != NULL, "field is NULL");

	StringBuffer buffer;
	const UChar *b = MultiTermQuery::toString(field);

	buffer.append (b);
	_CLDELETE_CARRAY(b);
	buffer.append(_T("~"));

	buffer.appendFloat(minimumSimilarity, 1);

	return buffer.toString();
}

const UChar *FuzzyQuery::getQueryName() const
{
	//Func - Returns the name of the query
	//Pre  - true
	//post - The string FuzzyQuery has been returned

	return getClassName();
}
const UChar *FuzzyQuery::getClassName()
{
	//Func - Returns the name of the query
	//Pre  - true
	//post - The string FuzzyQuery has been returned

	return cStrClassName;
}

/**
 * Returns the minimum similarity that is required for this query to match.
 * @return float value between 0.0 and 1.0
 */
float_t FuzzyQuery::getMinSimilarity() const
{
	return minimumSimilarity;
}

FuzzyQuery::FuzzyQuery(const FuzzyQuery& clone):
		MultiTermQuery(clone)
{
	this->minimumSimilarity = clone.getMinSimilarity();
	this->prefixLength = clone.getPrefixLength();

	//if(prefixLength < 0)
	//	_CLTHROWA(CL_ERR_IllegalArgument,"prefixLength < 0");
	//else
	if (prefixLength >= clone.getTerm()->textLength())
		_CLTHROWA(CL_ERR_IllegalArgument, "prefixLength >= term.textLength()");
}

Query* FuzzyQuery::clone() const
{
	return _CLNEW FuzzyQuery(*this);
}
size_t FuzzyQuery::hashCode() const
{
	return 0;

	/*
	//todo: we should give the query a seeding value... but
	//need to do it for all hascode functions
	size_t val = Similarity::floatToByte(getBoost()) ^ getTerm()->hashCode();
	val ^= Similarity::floatToByte(this->getMinSimilarity());
	val ^= this->getPrefixLength();
	return val;
	*/
}

bool FuzzyQuery::equals(Query *other) const
{
	if (!(other->instanceOf(FuzzyQuery::getClassName())))
		return false;

	FuzzyQuery* fq = static_cast<FuzzyQuery *>(other);
	return (this->getBoost() == fq->getBoost())
	       && this->getMinSimilarity() == fq->getMinSimilarity()
	       && this->getPrefixLength() == fq->getPrefixLength()
	       && getTerm()->equals(fq->getTerm());
}

/**
 * Returns the prefix length, i.e. the number of characters at the start
 * of a term that must be identical (not fuzzy) to the query term if the query
 * is to match that term.
 */
size_t FuzzyQuery::getPrefixLength() const
{
	return prefixLength;
}

FilteredTermEnum* FuzzyQuery::getEnum(Searcher *inSearcher)
{
	Term* term = getTerm(false);
	FuzzyTermEnum* ret = _CLNEW FuzzyTermEnum(inSearcher, term, minimumSimilarity, prefixLength);
	return ret;
}

CL_NS_END
