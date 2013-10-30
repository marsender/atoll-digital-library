/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

QueryResolver.hpp

*******************************************************************************/

#ifndef __QueryResolver_HPP
#define __QueryResolver_HPP
//------------------------------------------------------------------------------

#include "../util/Entry.hpp"
#include "unicode/uchar.h"
#include <vector>
//------------------------------------------------------------------------------

namespace Lucene { namespace search {
	class Query;
}}

namespace Atoll
{

class Searcher;

//! Query resolver
/**
	Logic:
		- Resolve the query
		- Search the database according to the query tree
*/
class QueryResolver
{
private:
	Searcher *mSearcher;
	//EntrySet *mEntrySet; //!< QueryResolver search result

	Lucene::search::Query* RewriteQuery(Lucene::search::Query *inQuery);

	static void OrSetIntersection(EntrySet *inSet1, EntrySet *inSet2);
	static void AndSetIntersection(EntrySet *inSet1, EntrySet *inSet2);
	static void AndPageSetIntersection(EntrySet *inSet1, EntrySet *inSet2);
	static void PhraseSetIntersection(EntrySet *inSet1, EntrySet *inSet2, unsigned int inWindow);
	static void NearSetIntersection(EntrySet *inSet1, EntrySet *inSet2, unsigned int inWindow);

public:

	//! Query resolver constructor
	QueryResolver(Searcher *inSearcher);
	~QueryResolver();

	/**
	* Resolve a query object
	* @memory Caller must clean up
	*/
	EntrySet *Resolve(Lucene::search::Query *&ioQuery);

	/**
	* Resolve a term query
	* @memory Caller must clean up
	*/
	EntrySet *RevolveTermQuery(const UChar *inStrField, const UChar *inStrTerm, unsigned int inLength);

	//! Filter the first set with the second, witch is a document set
	static void FilterDocSetIntersection(EntrySet *inSet1, EntrySet *inSet2);

	//! Reduce the lists to an empty list
	static EntrySet *ReduceEmptyList(std::vector<EntrySet *> &inVector);
	//! Reduce the lists to the first one
	static EntrySet *ReduceFirstList(std::vector<EntrySet *> &inVector);
	//! Reduce the lists to one list containing all the positions
	static EntrySet *ReduceOrList(std::vector<EntrySet *> &inVector);
	//! Reduce the lists to one list containing the positions that are present in all lists
	static EntrySet *ReduceAndList(std::vector<EntrySet *> &inVector);
	//! Reduce the lists to one list containing the positions that are in the same page in all lists
	static EntrySet *ReduceAndPageList(std::vector<EntrySet *> &inVector);
	//! Reduce the lists to one list containing the positions that are in the same phrase in all lists
	static EntrySet *ReducePhraseList(std::vector<EntrySet *> &inVector, unsigned int inWindow);
	//! Reduce the lists to one list containing the positions that have a near position in all lists
	static EntrySet *ReduceNearList(std::vector<EntrySet *> &inVector, unsigned int inWindow);
};
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
