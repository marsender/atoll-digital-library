/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerItem.hpp

*******************************************************************************/

#ifndef __IndexerItem_HPP
#define __IndexerItem_HPP
//------------------------------------------------------------------------------

#include "IndexerConfigParam.hpp"
#include "../common/Containers.hpp"
#include "../util/WordEntry.hpp"
#include "../util/WordEntries.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Indexer for a single item of an xml node
/**
	Logic:
		- Bufferize the field and all the words inside the node during parsing
		- The indexer can ask to compute the words and field
		- Everything is stored in memory only
		- No database is used
*/
class IndexerItem
{
private:
	Common::UnicodeStringSet mTmpSet;
	WordEntriesSet mWordEntriesSet;
	WordEntryVector mWordEntryVector;

	// No need for copy and assignment
	IndexerItem(const IndexerItem &);
	IndexerItem &operator=(const IndexerItem &);

	void WordEntryToSet(WordEntry &inWordEntry);

public:
	std::string mIndexDbName; //!< Configuration: index db name
	eTypIndex mTypIndex; //!< Configuration: index type
	bool mIsLowerCase; //!< Configuration: lower case index ?
	UnicodeString mField;
	Entry mEntryStart;
	Entry mEntryEnd;

	IndexerItem(const std::string &inIndexDbName, eTypIndex inTypIndex, bool isLowerCase);
	~IndexerItem();

	//WordEntriesSet *GetWordEntriesSet() { return &mWordEntriesSet; }

	//! Add a word with it's position
	//! The word must not contains spaces, tabulation or line feed
	void IiAddWordEntry(WordEntry &inWordEntry);

	//! Add content to the field
	void IiAddFieldContent(const UChar *inStr, unsigned int inLength);

	//! Get the individual words to index
	WordEntriesSet *GetWordEntriesSetToIndex();

	//! Get the formated field to index
	bool GetFieldToIndex(UnicodeString &outStr);

	std::string ToString() const;
};

typedef std::vector <IndexerItem *> IndexerItemVector;
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
