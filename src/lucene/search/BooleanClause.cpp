/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#include "../Lucene.hpp"
#include "BooleanClause.hpp"
#include "Query.hpp"

CL_NS_DEF(search)

BooleanClause::BooleanClause(Query* q, const bool DeleteQuery, const bool req, const bool p)
	:query(q),
	required(req),
	prohibited(p),
	deleteQuery(DeleteQuery)
{
}

BooleanClause::BooleanClause(const BooleanClause& clone)
	:query(clone.query->clone()),
	required(clone.required),
	prohibited(clone.prohibited),
	deleteQuery(true)
{
}

BooleanClause::~BooleanClause()
{
	if (deleteQuery)
		_CLDELETE(query);
}

BooleanClause* BooleanClause::clone() const
{
	BooleanClause* ret = _CLNEW BooleanClause(*this);
	return ret;
}

bool BooleanClause::equals(const BooleanClause* other) const
{
	return this->query->equals(other->query)
		&& (this->required == other->required)
		&& (this->prohibited == other->prohibited);
}

size_t BooleanClause::hashCode() const
{
	return query->hashCode() ^ (this->required ? 1 : 0) ^ (this->prohibited ? 2 : 0);
}

CL_NS_END
