/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Searcher.hpp

*******************************************************************************/

#ifndef __Searcher_HPP
#define __Searcher_HPP
//------------------------------------------------------------------------------

#include "../util/Entry.hpp"
#include "unicode/uchar.h"
//------------------------------------------------------------------------------

namespace Lucene { namespace search {
	class Query;
	class TopDocs;
}}

namespace Lucene { namespace index {
	class Term;
	class TermEnum;
}}

namespace Atoll
{
class DbNdx;
class Collection;
class QueryResolver;

//! Database query search
/**
	Logic:
		- Resolve the query
		- Search the database according to the query tree
*/
class Searcher //: public Searchable
{
private:
	Collection *mCollection;
	EntrySet *mEntrySet; //!< Search result
	Lucene::search::Query *mQuery;

	//! Set the query to run
	void SetQuery(Lucene::search::Query *inQuery);

public:
	QueryResolver *mQueryResolver;

	Searcher(Collection *inCollection);
	~Searcher();

	//! Get the database to query according to a field, or null if the database doesn't exist
	DbNdx *GetFieldDatabase(const UChar *inStrField) const;

	//! Set the string query to run
	void SetQuery(const UChar *inStrField, const UChar *inStrQuery, int32_t inLength = -1);

	//! Get the query which has been set.
	const Lucene::search::Query *GetQuery() const;

	//! Get the EntrySet
	const EntrySet *GetEntrySet() const;

	//! Compute the query
	void Compute(bool inTraceQuery = false);

	/** Returns an enumeration of all the terms in the index.
	* The enumeration is ordered by Term.compareTo(). Each term
	* is greater than all that precede it in the enumeration.
	* @memory Caller must clean up
	*/
	//Lucene::index::TermEnum* GetTermEnum() const;

	/** Returns an enumeration of all terms after a given term.
	* The enumeration is ordered by Term.compareTo(). Each term
	* is greater than all that precede it in the enumeration.
	* If no term is specified, an enumeration of all the Terms
	* and TermInfos in the set is returned.
	* @memory Caller must clean up
	*/
	Lucene::index::TermEnum* GetTermEnum(const Lucene::index::Term* inTerm) const;
};
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
