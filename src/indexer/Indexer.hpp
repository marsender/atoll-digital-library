/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Indexer.hpp

*******************************************************************************/

#ifndef __Indexer_HPP
#define __Indexer_HPP
//------------------------------------------------------------------------------

#include "IndexerNode.hpp"
#include "../common/Containers.hpp"
#include "../util/WordEntries.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{
class Adorner;
class DbManager;
class IndexerConfig;
class IndexerConfigParam;
class StringTokenizer;
class WordEntry;

//! Xml content indexer with adorner and database storage
/**
	Logic:
		- Recieve notification from the parser content handler
		- Each node indexation type (word, field or interval) can be configured with the IndexerConfig
		- Adorn the text content and send full words to the indexer
		- Indexed content is stored to database
*/
class Indexer
{
private:
	typedef std::map <std::string, WordEntriesSet *> WordEntriesSetMap;

	IndexerConfig *mIndexerConfig; //<! Indexer configuration
	//! List of the xml nodes index info
	IndexerNodeList *mIndexerNodeList;
	IndexerNodeList *mIndexerNodeList2;
	WordEntriesSetMap *mWordEntriesSetMapCache; //!< Optional cache

	DbManager *mDbMgr;
	Adorner *mAdorner;
	unsigned int mNumDoc;
	unsigned long mNbError;	//!< Number of errors in the document

	//! Store the word entries set cache into database. This method can be called at any time, to flush the cache
	void StoreDbNdxCache();
	//! Clear the word entries set cache
	void ClearDbNdxCache();

	//! Clear the indexer node list
	void ClearIndexerNodeList();

	//! Add content to the fields of all indexer nodes
	void IndexerAddFieldContent(const UChar *inStr, unsigned int inLength);

	//! Store a word entry to database
	void StoreWordEntryToDb(const std::string &inDbName, const UnicodeString &inWord, const Entry &inEntry);
	//! Store a word entry to the cache
	void StoreWordEntryToCache(const std::string &inDbName, const UnicodeString &inWord, const Entry &inEntry);

	//! Store a word set to database
	void StoreWordSetToDb(const std::string &inDbName, const WordEntriesSet *inWordEntriesSet);
	//! Store a word set to the cache
	void StoreWordSetToCache(const std::string &inDbName, const WordEntriesSet *inWordEntriesSet);

	//! Store a node word set to database
	void StoreNodeToDb(IndexerNode *inIndexerNode);

public:
	static bool mStaticTraceStoreDb;

	Indexer();
	~Indexer();

	//! Init the db manager. Must be set before parsing
	void SetDbManager(DbManager *inDbMgr);

	void StartNode(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap);
	void EndNode();

	//! Receive notification of character data inside an element
	void AddCharacters(const UChar *inStr, unsigned int inLength);

	//! Receive notification of the beginning of the document
	void StartDocument();

	//! Receive notification of the end of the document
	void EndDocument(bool inIsException);

	//! Get the number of error in the current document
	unsigned long GetNbError() const { return mNbError; }

	//! Load the indexer configuration
	void LoadIndexerConfig(const std::string &inFileName);
	//! Get an indexer config parameter from it's name
	IndexerConfigParam *GetIndexerConfigParam(const std::string &inIndexName);

	//! Load the record breaker configuration
	void LoadRecordBreakerConfig(const std::string &inFileName);

	//! Get the document number
	unsigned int GetNumDoc() const { return mNumDoc; }
	//! Init the document number. Must be set before parsing
	void SetIndexerDocRef(unsigned int inNumDoc);
	//! Init the destination file name for the adorner
	void SetIndexerAdornerDstFileName(const std::string &inFileName);
	//! Activate the record breaker
	void EnableIndexerOutputRecordBreaker(bool inEnable);
	//! Activate the document storage into database
	void EnableIndexerOutputDbDocStorage(bool inEnable);

	//! Activate the current indexer nodes for content indexing
	void IndexerActivateNodes();

	//! Add a word entry in all indexer nodes
	//! The word must not contains spaces, tabulation or line feed
	void IndexerAddWordEntry(WordEntry &inWordEntry);

	void StoreNodeListToDb(bool inFirstList);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
