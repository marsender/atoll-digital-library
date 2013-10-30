/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerNode.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerNode.hpp"
#include "../common/StdString.hpp"
#include "../util/Entry.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

IndexerNode::IndexerNode(IndexerItemVector *inIndexerItemVector)
{
	mIndexerItemVector = inIndexerItemVector; // May be null if there is nothing to index on this xml node
	mIndexerNodeState = eTypIndexerNodeNew;
}
//------------------------------------------------------------------------------

IndexerNode::~IndexerNode()
{
	if (mIndexerItemVector) {
		// Delete the indexer item vector
		IndexerItem *indexerItem;
		IndexerItemVector::iterator it, itEnd;
		for (it = mIndexerItemVector->begin(), itEnd = mIndexerItemVector->end(); it != itEnd; ++it) {
			indexerItem = *it;
			delete indexerItem;
		}
		delete mIndexerItemVector;
	}
}
//------------------------------------------------------------------------------

/*
// Iterate the indexer vector and set the current position for each indexer item
void IndexerNode::InSetNodePosition(Entry &inEntry)
{
	if (!mIndexerItemVector)
		return;

	// Iterate on the indexer items
	IndexerItem *indexerItem;
	IndexerItemVector::iterator it, itEnd;
	for (it = mIndexerItemVector->begin(), itEnd = mIndexerItemVector->end(); it != itEnd; ++it) {
		indexerItem = *it;
		indexerItem->mField.mEntry = inEntry;
	}
}*/
//------------------------------------------------------------------------------

void IndexerNode::InAddWordEntry(WordEntry &inWordEntry)
{
	if (!mIndexerItemVector)
		return;

	// Iterate on the indexer items
	IndexerItem *indexerItem;
	IndexerItemVector::iterator it = mIndexerItemVector->begin();
	IndexerItemVector::iterator itEnd = mIndexerItemVector->end();
	for (; it != itEnd; ++it) {
		indexerItem = *it;
		// Send the word entry for all types of index
		indexerItem->IiAddWordEntry(inWordEntry);
	}
}
//------------------------------------------------------------------------------

void IndexerNode::InAddAddFieldContent(const UChar *inStr, unsigned int inLength)
{
	if (!mIndexerItemVector)
		return;

	// Iterate on the indexer items
	IndexerItem *indexerItem;
	IndexerItemVector::iterator it = mIndexerItemVector->begin();
	IndexerItemVector::iterator itEnd = mIndexerItemVector->end();
	for (; it != itEnd; ++it) {
		indexerItem = *it;
		// Send the field for field index only
		if (indexerItem->mTypIndex == eTypIndexField)
			indexerItem->IiAddFieldContent(inStr, inLength);
	}
}
//------------------------------------------------------------------------------

std::string IndexerNode::ToString() const
{
	std::string s("IndexerNode:");

	s += " - State: ";
	switch (mIndexerNodeState) {
	case eTypIndexerNodeNew:
		s += "new";
		break;
	case eTypIndexerNodeActive:
		s += "active";
		break;
	case eTypIndexerNodeClose:
		s += "close";
		break;
	default:
		s += "unknown";
		break;
	}
	s += " - IndexerItemVector: ";
	if (mIndexerItemVector) {
		unsigned int nbEnt = mIndexerItemVector->size();
		if (nbEnt == 0) {
			s += "(empty)";
		}
		else	{
			char buf[50];
			sprintf (buf, "(count = %u)", nbEnt);
			s += buf;
			s += " [ ";
			// Iterate on the indexer items
			IndexerItem *indexerItem;
			IndexerItemVector::iterator it = mIndexerItemVector->begin();
			IndexerItemVector::iterator itEnd = mIndexerItemVector->end();
			for (; it != itEnd; ++it) {
				indexerItem = *it;
				s += indexerItem->ToString();
				s += " ";
			}
			s += "]";
		}
	}
	else
		s += "null";

	return s;
}
//------------------------------------------------------------------------------
