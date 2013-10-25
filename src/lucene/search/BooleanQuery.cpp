/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "BooleanQuery.hpp"
//#include "BooleanClause.hpp"
//#include "../index/IndexReader.hpp"
#include "../util/StringBuffer.hpp"
#include "../util/Arrays.hpp"
//#include "SearchHeader.hpp"
//#include "BooleanScorer.hpp"
//#include "Scorer.hpp"
//#include "../../searcher/Searcher.hpp"
#include "../../searcher/QueryResolver.hpp"
#include "unicode/ustring.h"

U_STRING_DECL(cStrClassName, "BooleanQuery", 12);
CL_NS_DEF(search)
void BooleanQuery::InitStringDecl() {
	U_STRING_INIT(cStrClassName, "BooleanQuery", 12);
}
CL_NS_END

CL_NS_USE(index)
CL_NS_USE(util)
using namespace Atoll;
using namespace Common;

CL_NS_DEF(search)

BooleanQuery::BooleanQuery():
		clauses(true)
{
}

BooleanQuery::BooleanQuery(const BooleanQuery& clone):
		Query(clone)
{
	size_t size = clone.clauses.size();
	for (uint32_t i = 0; i < size; i++) {
		BooleanClause* clause = clone.clauses[i]->clone();
		clause->deleteQuery = true;
		add(clause);
	}
}

BooleanQuery::~BooleanQuery()
{
	clauses.clear();
}

size_t BooleanQuery::hashCode() const
{
	//todo: do cachedHashCode, and invalidate on add/remove clause
	size_t ret = 0;
	size_t size = clauses.size();
	for (uint32_t i = 0 ; i < size; i++) {
		BooleanClause* c = clauses[i];
		ret = 31 * ret + c->hashCode();
	}
	//ret = ret ^ Similarity::floatToByte(getBoost());
	return ret;
}

const UChar *BooleanQuery::getQueryName() const
{
	return getClassName();
}
const UChar *BooleanQuery::getClassName()
{
	return cStrClassName;
}

/**
* Default value is 1024.  Use <code>org.apache.lucene.maxClauseCount</code>
* system property to override.
*/
size_t BooleanQuery::maxClauseCount = LUCENE_BOOLEANQUERY_MAXCLAUSECOUNT;
size_t BooleanQuery::getMaxClauseCount()
{
	return maxClauseCount;
}

void BooleanQuery::setMaxClauseCount(size_t maxClauseCount)
{
	BooleanQuery::maxClauseCount = maxClauseCount;
}

void BooleanQuery::add(Query* query, const bool deleteQuery, const bool required, const bool prohibited)
{
	BooleanClause* bc = _CLNEW BooleanClause(query, deleteQuery, required, prohibited);
	try {
		add(bc);
	}
	catch (...) {
		_CLDELETE(bc);
		throw;
	}
}

void BooleanQuery::add(BooleanClause* clause)
{
	if (clauses.size() >= getMaxClauseCount())
		_CLTHROWA(CL_ERR_TooManyClauses, "Too Many Clauses");

	clauses.push_back(clause);
}


size_t BooleanQuery::getClauseCount() const
{
	return (int32_t)clauses.size();
}

UChar *BooleanQuery::toString(const UChar *field) const
{
	StringBuffer buffer;
	if (getBoost() != 1.0) {
		buffer.append(_T("("));
	}

	UChar *buf;
	BooleanClause* c;
	bool isBooleanQuery;
	size_t size = clauses.size();
	for (uint32_t i = 0 ; i < size; i++) {
		c = clauses[i];
		isBooleanQuery = c->query->instanceOf(BooleanQuery::getClassName());

		if (c->required) // AND clause
			buffer.append(_T("+"));
		else if (c->prohibited) // NOT clause
			buffer.append(_T("-"));

		if (isBooleanQuery) // wrap sub-bools in parens
			buffer.append(_T("("));

		buf = c->query->toString(field);
		buffer.append(buf);
		_CLDELETE_CARRAY(buf);

		if (isBooleanQuery) // wrap sub-bools in parens
			buffer.append(_T(")"));

		if (i != size - 1)
			buffer.append(_T(" "));

		if (getBoost() != 1.0) {
			buffer.append(_T(")^"));
			buffer.appendFloat(getBoost(), 1);
		}
	}
	return buffer.toString();
}

void BooleanQuery::getClauses(BooleanClause** ret) const
{
	size_t size = clauses.size();
	for (uint32_t i = 0;i < size; i++)
		ret[i] = clauses[i];
}

Query* BooleanQuery::rewrite(Searcher *inSearcher)
{
	size_t size = clauses.size();

	// Optimize 1-clause queries
	if (size == 1) {
		BooleanClause* c = clauses[0];
		// Just return clause
		if (!c->prohibited) {
			// Rewrite first
			Query* query = c->query->rewrite(inSearcher);
			// If the query doesn't actually get re-written, then return a clone
			// (because the BooleanQuery will register different to the returned query)
			if (query == c->query)
				query = query->clone();
			// Incorporate boost
			if (getBoost() != 1.0f) {
				query->setBoost(getBoost() * query->getBoost());
			}
			return query;
		}
	}

	// Recursively rewrite
	BooleanQuery* clone = NULL;
	for (uint32_t i = 0 ; i < size; i++) {
		BooleanClause* c = clauses[i];
		Query* query = c->query->rewrite(inSearcher);
		if (query != c->query) {
			// Clause rewrote: must clone
			if (clone == NULL)
				clone = static_cast<BooleanQuery *>(this->clone());
			// todo: check if delete query should be on...
			// in fact we should try and get rid of these for compatibility sake
			clone->clauses.set(i, _CLNEW BooleanClause(query, true, c->required, c->prohibited));
		}
	}
	if (clone != NULL) {
		return clone; // Some clauses rewrote
	}
	else
		return this; // No clauses rewrote
}

Query* BooleanQuery::clone() const
{
	BooleanQuery* clone = _CLNEW BooleanQuery(*this);
	return clone;
}

bool BooleanQuery::equals(Query* o)const
{
	if (!(o->instanceOf(BooleanQuery::getClassName())))
		return false;

	const BooleanQuery* other = static_cast<BooleanQuery *>(o);
	bool ret = (this->getBoost() == other->getBoost());
	if (ret) {
		CLListEquals<BooleanClause, BooleanClause::Compare, const ClausesType, const ClausesType> comp;
		ret = comp.equals(&this->clauses, &other->clauses);
	}

	return ret;
}

Atoll::EntrySet* BooleanQuery::Search(Searcher *inSearcher)
{
	BooleanClause *c;
	EntrySet *entrySet = NULL;
	std::vector<EntrySet *> list;
	size_t size = clauses.size();
	for (uint32_t i = 0 ; i < size; i++) {
		c = clauses[i];
		entrySet = c->query->Search(inSearcher);
		list.push_back(entrySet);
		if (list.size() > 1) {
			if (c->required) // AND clause
				entrySet = QueryResolver::ReduceAndPageList(list); // not ReduceAndList
			else if (c->prohibited) // NOT clause
				entrySet = QueryResolver::ReduceEmptyList(list); // Todo
			else // OR clause
				entrySet = QueryResolver::ReduceOrList(list);
		}
	}
	if (!entrySet) {
		if (size)
			gLog.log(eTypLogError, "Err > Empty boolean query");
		entrySet = new EntrySet;
	}
	
	return entrySet;
}

/*
float_t BooleanQuery::BooleanWeight::getValue()
{
	return parentQuery->getBoost();
}
Query* BooleanQuery::BooleanWeight::getQuery()
{
	return (Query*)parentQuery;
}

BooleanQuery::BooleanWeight::BooleanWeight(Searcher* searcher,
    CLVector<BooleanClause*, Deletor::Object<BooleanClause> >* clauses, BooleanQuery* parentQuery)
{
	this->searcher = searcher;
	this->parentQuery = parentQuery;
	this->clauses = clauses;
	for (uint32_t i = 0 ; i < clauses->size(); i++) {
		weights.push_back((*clauses)[i]->query->_createWeight(searcher));
	}
}
BooleanQuery::BooleanWeight::~BooleanWeight()
{
	this->weights.clear();
}

float_t BooleanQuery::BooleanWeight::sumOfSquaredWeights()
{
	float_t sum = 0.0f;
	for (uint32_t i = 0 ; i < weights.size(); i++) {
		BooleanClause* c = (*clauses)[i];
		Weight* w = weights[i];
		if (!c->prohibited)
			sum += w->sumOfSquaredWeights();         // sum sub weights
	}
	sum *= parentQuery->getBoost() * parentQuery->getBoost();             // boost each sub-weight
	return sum ;
}

void BooleanQuery::BooleanWeight::normalize(float_t norm)
{
	norm *= parentQuery->getBoost();                         // incorporate boost
	for (uint32_t i = 0 ; i < weights.size(); i++) {
		BooleanClause* c = (*clauses)[i];
		Weight* w = weights[i];
		if (!c->prohibited)
			w->normalize(norm);
	}
}

Scorer* BooleanQuery::BooleanWeight::scorer(IndexReader* reader)
{
	// First see if the (faster) ConjunctionScorer will work.  This can be
	// used when all clauses are required.  Also, at this point a
	// BooleanScorer cannot be embedded in a ConjunctionScorer, as the hits
	// from a BooleanScorer are not always sorted by document number (sigh)
	// and hence BooleanScorer cannot implement skipTo() correctly, which is
	// required by ConjunctionScorer.
	bool allRequired = true;
	bool noneBoolean = true;
	{ //msvc6 scope fix
		for (uint32_t i = 0 ; i < weights.size(); i++) {
			BooleanClause* c = (*clauses)[i];
			if (!c->required)
				allRequired = false;
			if (c->query->instanceOf(BooleanQuery::getClassName()))
				noneBoolean = false;
		}
	}

	if (allRequired && noneBoolean) {           // ConjunctionScorer is okay
		ConjunctionScorer* result =
		  _CLNEW ConjunctionScorer(parentQuery->getSimilarity(searcher));
		for (uint32_t i = 0 ; i < weights.size(); i++) {
			Weight* w = weights[i];
			Scorer* subScorer = w->scorer(reader);
			if (subScorer == NULL)
				return NULL;
			result->add(subScorer);
		}
		return result;
	}

	// Use good-old BooleanScorer instead.
	BooleanScorer* result = _CLNEW BooleanScorer(parentQuery->getSimilarity(searcher));

	{ //msvc6 scope fix
		for (uint32_t i = 0 ; i < weights.size(); i++) {
			BooleanClause* c = (*clauses)[i];
			Weight* w = weights[i];
			Scorer* subScorer = w->scorer(reader);
			if (subScorer != NULL)
				result->add(subScorer, c->required, c->prohibited);
			else if (c->required)
				return NULL;
		}
	}

	return result;
}

void BooleanQuery::BooleanWeight::explain(IndexReader* reader, int32_t doc, Explanation* result)
{
	int32_t coord = 0;
	int32_t maxCoord = 0;
	float_t sum = 0.0f;
	Explanation* sumExpl = _CLNEW Explanation;
	for (uint32_t i = 0 ; i < weights.size(); i++) {
		BooleanClause* c = (*clauses)[i];
		Weight* w = weights[i];
		Explanation* e = _CLNEW Explanation;
		w->explain(reader, doc, e);
		if (!c->prohibited)
			maxCoord++;
		if (e->getValue() > 0) {
			if (!c->prohibited) {
				sumExpl->addDetail(e);
				sum += e->getValue();
				coord++;
				e = NULL; //prevent e from being deleted
			}
			else {
				//we want to return something else...
				_CLDELETE(sumExpl);
				result->setValue(0.0f);
				result->setDescription(_T("match prohibited"));
				return;
			}
		}
		else if (c->required) {
			_CLDELETE(sumExpl);
			result->setValue(0.0f);
			result->setDescription(_T("match prohibited"));
			return;
		}

		_CLDELETE(e);
	}
	sumExpl->setValue(sum);

	if (coord == 1) {                              // only one clause matched
		Explanation* tmp = sumExpl;
		sumExpl = sumExpl->getDetail(0)->clone();          // eliminate wrapper
		_CLDELETE(tmp);
	}

	sumExpl->setDescription(_T("sum of:"));
	float_t coordFactor = parentQuery->getSimilarity(searcher)->coord(coord, maxCoord);
	if (coordFactor == 1.0f) {                     // coord is no-op
		result->set(*sumExpl);                       // eliminate wrapper
		_CLDELETE(sumExpl);
	}
	else {
		result->setDescription( _T("product of:"));
		result->addDetail(sumExpl);

		StringBuffer explbuf;
		explbuf.append(_T("coord("));
		explbuf.appendInt(coord);
		explbuf.append(_T("/"));
		explbuf.appendInt(maxCoord);
		explbuf.append(_T(")"));
		result->addDetail(_CLNEW Explanation(coordFactor, explbuf.getBuffer()));
		result->setValue(sum*coordFactor);
	}
}
*/

CL_NS_END
