/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_BooleanClause_
#define _lucene_search_BooleanClause_

CL_NS_DEF(search)

class Query;

// A clause in a BooleanQuery.
class BooleanClause
{
public:
	class Compare: public CL_NS_STD(binary_function)<const BooleanClause*, const BooleanClause*, bool>
	{
	public:
		bool operator()(const BooleanClause* val1, const BooleanClause* val2) const {
			return val1->equals(val2);
		}
	};

	// The query whose matching documents are combined by the boolean query.
	Query* query;

	int32_t getClauseCount();

	// If true, documents documents which <i>do not</i>
	//	match this sub-query will <i>not</i> match the boolean query.
	bool required;

	// If true, documents documents which <i>do</i>
	//	match this sub-query will <i>not</i> match the boolean query.
	bool prohibited;

	bool deleteQuery;

	// Constructs a BooleanClause with query <code>q</code>, required
	//	<code>r</code> and prohibited <code>p</code>.
	BooleanClause(Query* q, const bool DeleteQuery, const bool req, const bool p);

	BooleanClause(const BooleanClause& clone);

	~BooleanClause();

	BooleanClause* clone() const;

	//! Returns true if object is equal to this
	bool equals(const BooleanClause* other) const;

	size_t hashCode() const;
};


CL_NS_END
#endif
