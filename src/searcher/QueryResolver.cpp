/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

QueryResolver.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "QueryResolver.hpp"
#include "Searcher.hpp"
#include "../database/DbNdx.hpp"
#include "../lucene/Lucene.hpp"
#include "../lucene/search/Query.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace Lucene::search;

QueryResolver::QueryResolver(Searcher *inSearcher)
	:mSearcher(inSearcher)
{
}
//------------------------------------------------------------------------------

QueryResolver::~QueryResolver()
{
}
//------------------------------------------------------------------------------

Query* QueryResolver::RewriteQuery(Query* inQuery)
{
	Query *query, *last, *rewrittenQuery;
	query = last = inQuery;
	for (rewrittenQuery = query->rewrite(mSearcher); rewrittenQuery != query;) {
		query = rewrittenQuery;
		if (query != last) { // && last != inQuery)
			_CLDELETE(last);
			last = query;
		}
		rewrittenQuery = query->rewrite(mSearcher);
	}

	return query;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::Resolve(Lucene::search::Query *&ioQuery)
{
	ioQuery = RewriteQuery(ioQuery);

	EntrySet *entrySet = ioQuery->Search(mSearcher);
	AppAssert(entrySet);

	return entrySet;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::RevolveTermQuery(const UChar *inStrField, const UChar *inStrTerm, unsigned int inLength)
{
	EntrySet *entrySet = new EntrySet();

	DbNdx *dbNdx = mSearcher->GetFieldDatabase(inStrField);
	if (dbNdx)
		dbNdx->GetWordEntries(inStrTerm, inLength, *entrySet);

	return entrySet;
}
//------------------------------------------------------------------------------

void QueryResolver::OrSetIntersection(EntrySet *inSet1, EntrySet *inSet2)
{
	EntrySet::const_iterator it = inSet2->begin(), itEnd = inSet2->end();
	for (; it != itEnd; ++it) {
		const Entry &e = *it;
		inSet1->insert(e);
	}
}
//------------------------------------------------------------------------------

void QueryResolver::AndSetIntersection(EntrySet *inSet1, EntrySet *inSet2)
{
	EntrySet::iterator it1 = inSet1->begin(), it1End = inSet1->end(),
		it2 = inSet2->begin(), it2End = inSet2->end();
	while (it1 != it1End && it2 != it2End) {
		const Entry &e1 = *it1;
		const Entry &e2 = *it2;
		if (e1 < e2) {
			inSet1->erase(it1++);
		}
		else if (e2 < e1) {
			++it2;
		}
		else { // e1 == e2
			++it1;
			++it2;
		}
	}
	// Anything left in inSet1 from here did not appear in inSet2, so we remove it
	inSet1->erase(it1, inSet1->end());
}
//------------------------------------------------------------------------------

void QueryResolver::FilterDocSetIntersection(EntrySet *inSet1, EntrySet *inSet2)
{
	EntrySet::iterator it1 = inSet1->begin(), it1End = inSet1->end(),
		it2 = inSet2->begin(), it2End = inSet2->end();
	while (it1 != it1End && it2 != it2End) {
		const Entry &e1 = *it1;
		const Entry &e2 = *it2;
		if (e1.mUiEntDoc < e2.mUiEntDoc) {
			inSet1->erase(it1++);
		}
		else if (e2.mUiEntDoc < e1.mUiEntDoc) {
			it2++;
		}
		else { // *it1 == *it2
			++it1;
		}
	}
	// Anything left in inSet1 from here doesn't filder inSet2, so we remove it
	inSet1->erase(it1, inSet1->end());
}
//------------------------------------------------------------------------------

void QueryResolver::AndPageSetIntersection(EntrySet *inSet1, EntrySet *inSet2)
{
	EntrySet::iterator it1 = inSet1->begin(), it1End = inSet1->end(),
		it2 = inSet2->begin(), it2End = inSet2->end();
	while (it1 != it1End && it2 != it2End) {
		const Entry &e1 = *it1;
		const Entry &e2 = *it2;
		// Document and page comparison first
		if (e1.mUiEntDoc != e2.mUiEntDoc || e1.mUlEntPge != e2.mUlEntPge) {
			if (*it1 < *it2) {
				inSet1->erase(it1++);
			}
			else if (*it2 < *it1) {
				inSet2->erase(it2++);
			}
		}
		else { // Same document and page
			++it1;
			++it2;
			// Check other matching postions in set 1
			//if (it2 == it2End) {
				while (it1 != it1End) {
					const Entry &e1 = *it1;
					if (e1.mUiEntDoc != e2.mUiEntDoc || e1.mUlEntPge != e2.mUlEntPge)
						break;
					++it1;
				}
			//}
			// Check other matching postions in set 2
			//if (it1 == it1End) {
				while (it2 != it2End) {
					const Entry &e2 = *it2;
					if (e1.mUiEntDoc != e2.mUiEntDoc || e1.mUlEntPge != e2.mUlEntPge)
						break;
					++it2;
				}
			//}
		}
	}
	// Anything left in inSet1 from here did not appear in inSet2, so we remove it
	inSet1->erase(it1, inSet1->end());
	inSet2->erase(it2, inSet2->end());
}
//------------------------------------------------------------------------------

void QueryResolver::PhraseSetIntersection(EntrySet *inSet1, EntrySet *inSet2, unsigned int inWindow)
{
	EntrySet::iterator it1 = inSet1->begin(), it1End = inSet1->end(),
		it2 = inSet2->begin(), it2End = inSet2->end();
	while (it1 != it1End && it2 != it2End) {
		const Entry &e1 = *it1;
		const Entry &e2 = *it2;
		// Document comparison first
		if (e1.mUiEntDoc != e2.mUiEntDoc) {
			if (*it1 < *it2) {
				inSet1->erase(it1++);
			}
			else if (*it2 < *it1) {
				inSet2->erase(it2++);
			}
		}
		else {
			// Same document => position comparison
			if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow) {
				inSet1->erase(it1++);
			}
			else if (e2.mUlEntPos < e1.mUlEntPos) {
				inSet2->erase(it2++);
			}
			else {
				++it1;
				++it2;
				// Check other matching postions in set 1
				//if (it2 == it2End) {
					while (it1 != it1End) {
						const Entry &e1 = *it1;
						if (e1.mUiEntDoc != e2.mUiEntDoc)
							break;
						if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow)
							break;
						if (e2.mUlEntPos < e1.mUlEntPos)
							break;
						++it1;
					}
				//}
				// Check other matching postions in set 2
				//if (it1 == it1End) {
					while (it2 != it2End) {
						const Entry &e2 = *it2;
						if (e1.mUiEntDoc != e2.mUiEntDoc)
							break;
						if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow)
							break;
						if (e2.mUlEntPos < e1.mUlEntPos)
							break;
						++it2;
					}
				//}
			}
		}
	}
	// Anything left in inSet1 from here did not appear in inSet2, so we remove it
	inSet1->erase(it1, inSet1->end());
	inSet2->erase(it2, inSet2->end());
}
//------------------------------------------------------------------------------

void QueryResolver::NearSetIntersection(EntrySet *inSet1, EntrySet *inSet2, unsigned int inWindow)
{
	EntrySet::iterator it1 = inSet1->begin(), it1End = inSet1->end(),
		it2 = inSet2->begin(), it2End = inSet2->end();
	while (it1 != it1End && it2 != it2End) {
		const Entry &e1 = *it1;
		const Entry &e2 = *it2;
		// Document comparison first
		if (e1.mUiEntDoc != e2.mUiEntDoc) {
			if (*it1 < *it2) {
				inSet1->erase(it1++);
			}
			else if (*it2 < *it1) {
				inSet2->erase(it2++);
			}
		}
		else {
			// Same document => position comparison
			if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow) {
				inSet1->erase(it1++);
			}
			else if (e2.mUlEntPos < e1.mUlEntPos && e1.mUlEntPos - e2.mUlEntPos > inWindow) {
				inSet2->erase(it2++);
			}
			else {
				++it1;
				++it2;
				// Check other matching postions in set 1
				//if (it2 == it2End) {
					while (it1 != it1End) {
						const Entry &e1 = *it1;
						if (e1.mUiEntDoc != e2.mUiEntDoc)
							break;
						if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow)
							break;
						if (e2.mUlEntPos < e1.mUlEntPos && e1.mUlEntPos - e2.mUlEntPos > inWindow)
							break;
						++it1;
					}
				//}
				// Check other matching postions in set 2
				//if (it1 == it1End) {
					while (it2 != it2End) {
						const Entry &e2 = *it2;
						if (e1.mUiEntDoc != e2.mUiEntDoc)
							break;
						if (e1.mUlEntPos < e2.mUlEntPos && e2.mUlEntPos - e1.mUlEntPos > inWindow)
							break;
						if (e2.mUlEntPos < e1.mUlEntPos && e1.mUlEntPos - e2.mUlEntPos > inWindow)
							break;
						++it2;
					}
				//}
			}
		}
	}
	// Anything left in inSet1 from here did not appear in inSet2, so we remove it
	inSet1->erase(it1, inSet1->end());
	inSet2->erase(it2, inSet2->end());
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceEmptyList(std::vector<EntrySet *> &inVector)
{
	EntrySet *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		delete entrySet;
	}
	pl->clear();
	inVector.clear();

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceFirstList(std::vector<EntrySet *> &inVector)
{
	EntrySet *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		delete entrySet;
	}
	inVector.clear();

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceOrList(std::vector<EntrySet *> &inVector)
{
	EntrySet *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		OrSetIntersection(pl, entrySet);
		delete entrySet;
	}
	inVector.clear();
	inVector.push_back(pl);

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceAndList(std::vector<EntrySet *> &inVector)
{
	EntrySet *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		AndSetIntersection(pl, entrySet);
		delete entrySet;
	}
	inVector.clear();
	inVector.push_back(pl);

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceAndPageList(std::vector<EntrySet *> &inVector)
{
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		AndPageSetIntersection(pl, entrySet);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		OrSetIntersection(plFirst, entrySet);
		pl = entrySet;
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else
		pl = ReduceOrList(inVector);

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReducePhraseList(std::vector<EntrySet *> &inVector, unsigned int inWindow)
{
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		PhraseSetIntersection(pl, entrySet, 1);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		if (pl != plFirst)
			PhraseSetIntersection(plFirst, entrySet, inWindow);
		OrSetIntersection(plFirst, entrySet);
		pl = entrySet;
		inWindow++;
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else
		pl = ReduceFirstList(inVector);

	/*
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		PhraseSetIntersection(plFirst, entrySet, inWindow++);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		OrSetIntersection(plFirst, entrySet);
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else
		pl = ReduceFirstList(inVector);
	*/

	/*
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		PhraseSetIntersection(pl, entrySet, inWindow);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		//OrSetIntersection(plFirst, entrySet);
		pl = entrySet;
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else {
		// Delete the lists, except the last one
		for (it = inVector.begin(); it != itEnd; ++it) {
			entrySet = *it;
			if (entrySet == pl)
				break;
			delete entrySet;
		}
		// Compute all the window positions
		pl = new EntrySet;
		unsigned int windowMax = inVector.size();
		EntrySet::iterator itEntry = entrySet->begin(), itEntryEnd = entrySet->end();
		for (; itEntry != itEntryEnd; ++itEntry) {
			Entry &e = *itEntry;
			pl->insert(e);
			for (unsigned int i = 1; i < windowMax; i++) {
				e.mUlEntPos--;
				pl->insert(e);
			}
		}
		delete entrySet;
		inVector.clear();
	}
	*/

	/*
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		PhraseSetIntersection(pl, entrySet, inWindow);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		OrSetIntersection(plFirst, entrySet);
		pl = entrySet;
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else
		pl = ReduceOrList(inVector);
	*/

	return pl;
}
//------------------------------------------------------------------------------

EntrySet *QueryResolver::ReduceNearList(std::vector<EntrySet *> &inVector, unsigned int inWindow)
{
	bool isEmpty = false;
	EntrySet *plFirst, *pl, *entrySet;
	std::vector<EntrySet *>::iterator it = inVector.begin(), itEnd = inVector.end();
	plFirst = pl = *it; // Keep the first list
	for (it++; it != itEnd; ++it) {
		entrySet = *it;
		NearSetIntersection(pl, entrySet, inWindow);
		if (entrySet->empty()) {
			isEmpty = true;
			break;
		}
		if (pl != plFirst)
			NearSetIntersection(plFirst, entrySet, inWindow);
		OrSetIntersection(plFirst, entrySet);
		pl = entrySet;
		//inWindow++;
	}

	if (isEmpty)
		pl = ReduceEmptyList(inVector);
	else
		pl = ReduceOrList(inVector);

	return pl;
}
//------------------------------------------------------------------------------
