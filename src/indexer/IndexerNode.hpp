/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerNode.hpp

*******************************************************************************/

#ifndef __IndexerNode_HPP
#define __IndexerNode_HPP
//------------------------------------------------------------------------------

#include "IndexerItem.hpp"
#include "unicode/unistr.h"
#include <list>
#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{
class Entry;
class WordEntry;

//! Xml node with it's vector of indexer items to index
/**
	Logic:
		- Store the list of all the current indexer items for a node
		- Everything is stored in the indexer vector
		- No database is used
*/
class IndexerNode
{
public:
	//! Btree key comparison type
	enum eTypIndexerNodeState {
		eTypIndexerNodeNone,
		eTypIndexerNodeNew, //!< Node open but not yet active
		eTypIndexerNodeActive, //!< Node active: allow indexation
		eTypIndexerNodeClose, //!< Node closed
		eTypIndexerNodeEnd
	};

	eTypIndexerNodeState mIndexerNodeState;
	IndexerItemVector *mIndexerItemVector; //!< Vector of indexer items or NULL

	IndexerNode(IndexerItemVector *inIndexerItemVector);
	~IndexerNode();

	//! Add a word entry in all indexer items of the node
	//! The word must not contains spaces, tabulation or line feed
	void InAddWordEntry(WordEntry &inWordEntry);

	//! Add field content in all indexer items of the node
	void InAddAddFieldContent(const UChar *inStr, unsigned int inLength);

	std::string ToString() const;
};

// List is mandatory as we use pop_front (not present in vector)
typedef std::list <IndexerNode *> IndexerNodeList;
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
