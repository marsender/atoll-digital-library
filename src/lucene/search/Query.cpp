/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "Query.hpp"
#include "BooleanQuery.hpp"
//#include "BooleanClause.hpp"
#include "../util/VoidList.hpp"
//#include "FieldDocSortedHitQueue.hpp"
#include "unicode/ustring.h"

//CL_NS_USE(index)
CL_NS_USE(util)
using namespace Common;

CL_NS_DEF(search)

//static
Query* Query::mergeBooleanQueries(Query** queries)
{
	CL_NS(util)::CLVector<BooleanClause*> allClauses;
	int32_t i = 0;
	while (queries[i] != NULL) {
		BooleanQuery* bq = static_cast<BooleanQuery *>(queries[i]);

		int32_t size = bq->getClauseCount();
		BooleanClause** clauses = _CL_NEWARRAY(BooleanClause*, size);
		bq->getClauses(clauses);

		for (int32_t j = 0;j < size; ++j) {
			allClauses.push_back(clauses[j]);
			j++;
		}
		_CLDELETE_ARRAY(clauses);
		i++;
	}

	BooleanQuery* result = _CLNEW BooleanQuery();
	CL_NS(util)::CLVector<BooleanClause*>::iterator itr = allClauses.begin();
	while (itr != allClauses.end()) {
		result->add(*itr);
	}
	return result;
}

Query::Query(const Query& clone): boost(clone.boost)
{
	//constructor
}

/*
Weight* Query::_createWeight(Searcher* searcher)
{
	_CLTHROWA(CL_ERR_UnsupportedOperation, "UnsupportedOperationException: Query::_createWeight");
}
*/

Query::Query():
		boost(1.0f)
{
	//constructor
}
Query::~Query()
{
}

/** Expert: called to re-write queries into primitive queries. */
Query* Query::rewrite(Atoll::Searcher *inSearcher)
{
	return this;
}

Query* Query::combine(Query** queries)
{
	_CLTHROWA(CL_ERR_UnsupportedOperation, "UnsupportedOperationException: Query::combine");
}

/*
Similarity* Query::getSimilarity(Searcher* searcher)
{
	return searcher->getSimilarity();
}
*/

bool Query::instanceOf(const UChar *other) const
{
	const UChar *t = getQueryName();
	if (t == other || _tcscmp(t, other) == 0)
		return true;
	else
		return false;
}

UChar *Query::toString() const
{
	return toString(LUCENE_BLANK_STRING);
}

void Query::setBoost(float_t b)
{
	boost = b;
}

float_t Query::getBoost() const
{
	return boost;
}

/*
Weight* Query::weight(Searcher* searcher)
{
	Query* query = searcher->rewrite(this);
	Weight* weight = query->_createWeight(searcher);
	float_t sum = weight->sumOfSquaredWeights();
	float_t norm = getSimilarity(searcher)->queryNorm(sum);
	weight->normalize(norm);
	return weight;
}
*/

CL_NS_END
