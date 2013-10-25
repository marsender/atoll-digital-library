/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "MultiTermQuery.hpp"
#include "TermQuery.hpp"
#include "FilteredTermEnum.hpp"
#include "BooleanQuery.hpp"
//#include "BooleanClause.hpp"
#include "../util/StringBuffer.hpp"
//#include "../index/IndexReader.hpp"
#include "../index/Term.hpp"
//#include "../index/Terms.hpp"
#include "unicode/ustring.h"

CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)
using namespace Atoll;
using namespace Common;

/** Constructs a query for terms matching <code>term</code>. */

MultiTermQuery::MultiTermQuery(Term* t)
{
	//Func - Constructor
	//Pre  - t != NULL
	//Post - The instance has been created

	CND_PRECONDITION(t != NULL, "t is NULL");

	term  = _CL_POINTER(t);

}
MultiTermQuery::MultiTermQuery(const MultiTermQuery& clone):
		Query(clone)
{
	term = _CLNEW Term(clone.getTerm(false)->field(), clone.getTerm(false)->text());
}

MultiTermQuery::~MultiTermQuery()
{
	//Func - Destructor
	//Pre  - true
	//Post - The instance has been destroyed

	_CLDECDELETE(term);
}

Term* MultiTermQuery::getTerm(bool pointer) const
{
	if (pointer)
		return _CL_POINTER(term);
	else
		return term;
}

Query* MultiTermQuery::rewrite(Searcher *inSearcher)
{
	FilteredTermEnum* enumerator = getEnum(inSearcher);
	BooleanQuery* query = _CLNEW BooleanQuery();
	try {
		do {
			Term* t = enumerator->term(false);
			if (t != NULL) {
				TermQuery* tq = _CLNEW TermQuery(t);	// found a match
				tq->setBoost(getBoost() * enumerator->difference()); // set the boost
				query->add(tq, true, false, false);		// add to q
			}
		}
		while (enumerator->next());
	}
	_CLFINALLY(enumerator->close(); _CLDELETE(enumerator));

	// Optimize 1-clause queries
	size_t size = query->getClauseCount();
	if (size == 1) {
		BooleanClause* c = 0;
		query->getClauses(&c);
		// Just return clause
		if (!c->prohibited) {
			c->deleteQuery = false;
			Query* ret = c->query;
			_CLDELETE(query);
			return ret;
		}
	}
	return query;
}

Query* MultiTermQuery::combine(Query** queries)
{
	return Query::mergeBooleanQueries(queries);
}

UChar *MultiTermQuery::toString(const UChar *field) const
{
	StringBuffer buffer;

	if (field == NULL || _tcscmp(term->field(), field) != 0) {
		buffer.append(term->field());
		buffer.append( _T(":"));
	}
	buffer.append(term->text());
	if (getBoost() != 1.0f) {
		buffer.appendChar ('^');
		buffer.appendFloat(getBoost(), 1);
	}
	return buffer.toString();
}

Atoll::EntrySet* MultiTermQuery::Search(Searcher *inSearcher)
{
	// Invalid, as MultiTermQuery is rewriten as a BooleanQuery
	DEF_Exception(AppException::InvalidOperation, "MultiTermQuery::Search");

	return NULL;
}

CL_NS_END
