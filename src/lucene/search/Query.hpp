/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_Query_
#define _lucene_search_Query_

#include "../util/RefCount.hpp"
//#include "../index/IndexReader.hpp"
//#include "../index/Term.hpp"
//#include "Filter.hpp"
//#include "../document/Document.hpp"
//#include "Sort.hpp"
//#include "../util/VoidList.hpp"
//#include "Explanation.hpp"
//#include "Similarity.hpp"
#include "../../util/Entry.hpp"

namespace Atoll
{
class Searcher;
}

CL_NS_DEF(index)
class IndexReader;
CL_NS_END

CL_NS_DEF(search)

//predefine classes
//class Scorer;
//class Query;
//class Hits;
//class Sort;
//class FieldDoc;
//class TopFieldDocs;

/** The abstract base class for queries.
  Instantiable subclasses are:
  <ul>
  <li> {@link TermQuery}</li>
  <li> {@link MultiTermQuery}</li>
  <li> {@link BooleanQuery}</li>
  <li> {@link WildcardQuery}</li>
  <li> {@link PrefixQuery}</li>
  <li> {@link FuzzyQuery}</li>
  <li> {@link RangeQuery}</li>
  </ul>
  A parser for queries is contained in:
  <ul>
  <li>{@link queryParser.QueryParser QueryParser}</li>
  </ul>
*/
class Query: public CL_NS(util)::RefCount
{
private:
	float_t boost; //!< query boost factor

protected:
	Query(const Query& clone);
public:
	Query();
	virtual ~Query();

	/** Sets the boost for this query clause to <code>b</code>. Documents
	 * matching this clause will (in addition to the normal weightings) have
	 * their score multiplied by <code>b</code>.
	 */
	void setBoost(float_t b);

	/** Gets the boost for this clause. Documents matching
	 * this clause will (in addition to the normal weightings) have their score
	 * multiplied by <code>b</code>. The boost is 1.0 by default.
	 */
	float_t getBoost() const;

	/*
	 * Expert: Constructs an initializes a Weight for a top-level query.
	 */
	//Weight* weight(Searcher* searcher);

	virtual Atoll::EntrySet* Search(Atoll::Searcher *inSearcher) = 0;

	/** Expert: called to re-write queries into primitive queries. */
	virtual Query* rewrite(Atoll::Searcher *inSearcher);

	/** Expert: called when re-writing queries under MultiSearcher.
	 *
	 * Only implemented by derived queries, with no _createWeight implementatation
	 */
	virtual Query* combine(Query** queries);

	/** Expert: merges the clauses of a set of BooleanQuery's into a single
	 * BooleanQuery.
	 *
	 * A utility for use by combine(Query**)} implementations.
	 */
	static Query* mergeBooleanQueries(Query** queries);

	/* Expert: Returns the Similarity implementation to be used for this query.
	 * Subclasses may override this method to specify their own Similarity
	 * implementation, perhaps one that delegates through that of the Searcher.
	 * By default the Searcher's Similarity implementation is returned.
	 */
	//Similarity* getSimilarity(Searcher* searcher);

	/** Returns a clone of this query. */
	virtual Query* clone() const = 0;
	virtual const UChar *getQueryName() const = 0;
	bool instanceOf(const UChar *other) const;

	/** Prints a query to a string, with <code>field</code> as the default field
	 * for terms. The representation used is one that is readable by
	 * {@link queryParser.QueryParser QueryParser}
	 * (although, if the query was created by the parser, the printed
	 * representation may not be exactly what was parsed).
	 */
	virtual UChar *toString(const UChar *field) const = 0;

	virtual bool equals(Query *other) const = 0;
	virtual size_t hashCode() const = 0;

	/** Prints a query to a string. */
	UChar *toString() const;


	/* Expert: Constructs an appropriate Weight implementation for this query.
	 *
	 * Only implemented by primitive queries, which re-write to themselves.
	 * This is an Internal function
	 */
	//virtual Weight* _createWeight(Searcher* searcher);

};

CL_NS_END
#endif
