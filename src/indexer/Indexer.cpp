/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Indexer.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Indexer.hpp"
#include "IndexerConfig.hpp"
#include "IndexerItem.hpp"
#include "../adorner/Adorner.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbNdx.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

//#define DEF_WordSetCacheSize			DEF_LongMax // 1000 or DEF_LongMax
#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Indexer: %s", x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

bool Indexer::mStaticTraceStoreDb = false;

Indexer::Indexer()
{
	mNbError = 0;
	mDbMgr = NULL;

	mAdorner = new Adorner();
	mAdorner->SetAdornerIndexer(this);

	mIndexerConfig = new IndexerConfig();
	mIndexerNodeList = new IndexerNodeList();
	mIndexerNodeList2 = new IndexerNodeList();

	// Optional cache
	// This cache is very usefull, in order to reduce the databases storage access
	// If this cache grows too much, we could flush it automaticaly after a certain size
	mWordEntriesSetMapCache = new WordEntriesSetMap();

	mNumDoc = 0;
}
//------------------------------------------------------------------------------

Indexer::~Indexer()
{
	delete mAdorner;

	delete mIndexerConfig;

	ClearDbNdxCache();
	ClearIndexerNodeList();

	delete mIndexerNodeList;
	delete mIndexerNodeList2;
	delete mWordEntriesSetMapCache;

	// Check errors if the document has not been closed
  if (mNumDoc != 0)
		gLog.log(eTypLogError, "Err > Indexer document number not null: %u", mNumDoc);
}
//------------------------------------------------------------------------------

void Indexer::SetDbManager(DbManager *inDbMgr)
{
	mDbMgr = inDbMgr;
	mAdorner->SetDbManager(inDbMgr);
}
//------------------------------------------------------------------------------

void Indexer::LoadIndexerConfig(const std::string &inFileName)
{
	mIndexerConfig->LoadFromFile(inFileName);
}
//------------------------------------------------------------------------------

IndexerConfigParam *Indexer::GetIndexerConfigParam(const std::string &inIndexName)
{
	return mIndexerConfig->GetConfigParam(inIndexName);
}
//------------------------------------------------------------------------------

void Indexer::LoadRecordBreakerConfig(const std::string &inFileName)
{
	mAdorner->LoadAdornerRecordBreakerConfig(inFileName);
}
//------------------------------------------------------------------------------

void Indexer::SetIndexerDocRef(unsigned int inNumDoc)
{
	mNumDoc = inNumDoc;
	mAdorner->SetAdornerDocRef(inNumDoc);
}
//------------------------------------------------------------------------------

void Indexer::SetIndexerAdornerDstFileName(const std::string &inFileName)
{
	mAdorner->SetAdornerDstFileName(inFileName);
}
//------------------------------------------------------------------------------

void Indexer::EnableIndexerOutputRecordBreaker(bool inEnable)
{
	mAdorner->EnableAdornerRecordBreaker(inEnable);
}
//------------------------------------------------------------------------------

void Indexer::EnableIndexerOutputDbDocStorage(bool inEnable)
{
	mAdorner->EnableAdornerDbDocStorage(inEnable);
}
//------------------------------------------------------------------------------

void Indexer::StoreDbNdxCache()
{
	if (!mWordEntriesSetMapCache)
		return;

	WordEntriesSetMap::const_iterator it = mWordEntriesSetMapCache->begin();
	WordEntriesSetMap::const_iterator itEnd = mWordEntriesSetMapCache->end();
	for (; it != itEnd; ++it) {
		const std::string &dbName = it->first;
		const WordEntriesSet *wordEntriesSet = it->second;
		StoreWordSetToDb(dbName, wordEntriesSet);
	}

	// Clear the cache, as it has been used
	ClearDbNdxCache();
}
//------------------------------------------------------------------------------

void Indexer::ClearDbNdxCache()
{
	if (!mWordEntriesSetMapCache)
		return;

	WordEntriesSet::iterator itWes, itWesEnd;
	WordEntriesSetMap::iterator it = mWordEntriesSetMapCache->begin();
	WordEntriesSetMap::iterator itEnd = mWordEntriesSetMapCache->end();
	for (; it != itEnd; ++it) {
		WordEntriesSet *wordEntriesSet = it->second;
		/*
		itWes = wordEntriesSet->begin();
		itWesEnd = wordEntriesSet->end();
		for (; itWes != itWesEnd; ++itWes) {
			WordEntries *we = &(*itWes);
			delete we;
		}
		*/
		delete wordEntriesSet;
	}
	mWordEntriesSetMapCache->clear();
}
//------------------------------------------------------------------------------

void Indexer::ClearIndexerNodeList()
{
	IndexerNodeList::iterator it = mIndexerNodeList->begin();
	IndexerNodeList::iterator itEnd = mIndexerNodeList->end();
	for (; it != itEnd; ++it) {
		IndexerNode *indexerNode = *it;
		delete indexerNode;
	}
	mIndexerNodeList->clear();
	mIndexerNodeList2->clear();
}
//------------------------------------------------------------------------------

void Indexer::StartDocument()
{
  AppAssert(mDbMgr != NULL);
  AppAssert(mNumDoc != 0);

	mNbError = 0;

	mAdorner->StartDocument();

	// Clear the cache (should be already empty)
	ClearDbNdxCache();

	// Clear the node list (should be already empty)
	ClearIndexerNodeList();
}
//------------------------------------------------------------------------------

void Indexer::EndDocument(bool inIsException)
{
	mAdorner->EndDocument(inIsException);

	// Add indexer errors
	mNbError += mAdorner->GetNbError();

	if (!mIndexerNodeList->empty()) {
		if (!inIsException) {
			mNbError++;
			DEF_Log("EndDocument: node list not empty");
		}
	}
	if (!mIndexerNodeList2->empty()) {
		if (!inIsException) {
			mNbError++;
			DEF_Log("EndDocument: second node list not empty");
		}
	}

	// Send the cache to db
	StoreDbNdxCache();

	// Clear the document number
	mNumDoc = 0;
}
//------------------------------------------------------------------------------

void Indexer::StartNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap)
{
	bool wantTrace = false;

	// Send the start element notification to the adorner
	mAdorner->StartNode(inElem, inAttrMap);

	if (wantTrace)
		Trace("Indexer::StartNode: %s", inElem.c_str());

	// Get the vector of nodes to index, or NULL if there is nothing to index
	IndexerItemVector *indexerItemVector = mIndexerConfig->GetNodeIndexerVector(inElem, inAttrMap);

	// Create an indexer for the node
	IndexerNode *indexerNode = new IndexerNode(indexerItemVector);

	// Add the node in the list
	mIndexerNodeList->push_back(indexerNode);
}
//------------------------------------------------------------------------------

void Indexer::EndNode()
{
	// Send the end element notification to the adorner
	mAdorner->EndNode();

	// Close the node and move it to the second list
	IndexerNode *indexerNode = mIndexerNodeList->back();
	indexerNode->mIndexerNodeState = IndexerNode::eTypIndexerNodeClose;
	mIndexerNodeList->pop_back();
	mIndexerNodeList2->push_back(indexerNode);
}
//------------------------------------------------------------------------------

void Indexer::AddCharacters(const UChar *inStr, unsigned int inLength)
{
	mAdorner->AddCharacters(inStr, inLength);

	// Add the string in the fields of all indexer nodes
	IndexerAddFieldContent(inStr, inLength);

	bool wantTrace = false;
	if (wantTrace) {
		UTrace("Indexer::AddCharacters", inStr, inLength);
	}

	/*
	if (mAdorner->NeedTokenize()) {
		// Check if the characters are a line break
		// in order to tokenize the current string buffer
		UChar *pos = u_strchr(inStr, 0x000A);
		if (pos) {
			// Check if there isn't other chars than space, tab or crlf
			const UChar *sPos = inStr;
			bool isNewLine = true;
			while (*sPos != 0 && isNewLine) {
				//if (u_hasBinaryProperty(*sPos, UCHAR_LINE_BREAK))
				switch (*sPos) {
				case 0x0009: // Tab
				case 0x000A: // Cr
				case 0x0020: // Space
					break;
				default:
					isNewLine = false;
					break;
				}
				sPos++;
			}
			if (isNewLine)
				mAdorner->Tokenize();
		}
	}
	*/
}
//------------------------------------------------------------------------------

void Indexer::IndexerActivateNodes()
{
	IndexerNodeList::iterator it, itEnd;
	it = mIndexerNodeList->begin();
	itEnd = mIndexerNodeList->end();
	for (; it != itEnd; ++it) {
		IndexerNode *indexerNode = *it;
		indexerNode->mIndexerNodeState = IndexerNode::eTypIndexerNodeActive;
	}
}
//------------------------------------------------------------------------------

void Indexer::IndexerAddWordEntry(WordEntry &inWordEntry)
{
	size_t count;
	IndexerNode *indexerNode;
	IndexerNodeList *nodeList;
	IndexerNodeList::iterator it, itEnd;

	for (int i = 0; i <= 1; ++i) {
		nodeList = (i == 0) ? mIndexerNodeList : mIndexerNodeList2;
		count = nodeList->size();
		if (count == 0)
			continue;
		it = nodeList->begin();
		itEnd = nodeList->end();
		for (; it != itEnd; ++it) {
			indexerNode = *it;
			if (indexerNode->mIndexerNodeState == IndexerNode::eTypIndexerNodeNew)
				continue;
			indexerNode->InAddWordEntry(inWordEntry);
		}
	}
}
//------------------------------------------------------------------------------

void Indexer::IndexerAddFieldContent(const UChar *inStr, unsigned int inLength)
{
	IndexerNodeList::iterator it = mIndexerNodeList->begin();
	IndexerNodeList::iterator itEnd = mIndexerNodeList->end();
	for (; it != itEnd; ++it) {
		IndexerNode *indexerNode = *it;
		indexerNode->InAddAddFieldContent(inStr, inLength);
	}
}
//------------------------------------------------------------------------------

void Indexer::StoreWordEntryToDb(const std::string &inDbName,
																 const UnicodeString &inWord,
																 const Entry &inEntry)
{
	bool wantTrace = mStaticTraceStoreDb;

	if (wantTrace) {
		WordEntry we(inWord, inEntry);
		std::string strTmp = we.ToString();
		Trace("Indexer::StoreWordEntryToDb - Db name: %s - %s", inDbName.c_str(), strTmp.c_str());
	}

	DbNdx *dbNdx = mDbMgr->GetDbNdx(inDbName);

	dbNdx->AddWordEntry(inWord, inEntry);
}
//------------------------------------------------------------------------------

void Indexer::StoreWordEntryToCache(const std::string &inDbName,
																 const UnicodeString &inWord,
																 const Entry &inEntry)
{
	bool wantTrace = mStaticTraceStoreDb;

	if (wantTrace) {
		WordEntry we(inWord, inEntry);
		std::string strTmp = we.ToString();
		Trace("Indexer::StoreWordEntryToCache - Db name: %s - %s", inDbName.c_str(), strTmp.c_str());
	}

	WordEntries wordEntries;
	WordEntriesSet wordEntriesSet;
	wordEntries.mWrd = inWord;
	wordEntries.AddEntry(inEntry);
	wordEntriesSet.insert(wordEntries);

	StoreWordSetToCache(inDbName, &wordEntriesSet);
}
//------------------------------------------------------------------------------

void Indexer::StoreWordSetToDb(const std::string &inDbName, const WordEntriesSet *inWordEntriesSet)
{
	bool wantTrace = mStaticTraceStoreDb;

	if (inWordEntriesSet->empty())
		return;

	if (false && wantTrace) {
		size_t setCount = inWordEntriesSet->size();
		Trace("Indexer::StoreWordSetToDb begin (%ld words)", setCount);
	}

	DbNdx *dbNdx;
	dbNdx = mDbMgr->GetDbNdx(inDbName);

	// Debug check info only
	//unsigned long dbNdxCount = dbNdx->Count();
	//size_t numWord = 0, entryCount;

	WordEntriesSet::const_iterator it = inWordEntriesSet->begin();
	WordEntriesSet::const_iterator itEnd = inWordEntriesSet->end();
	for (; it != itEnd; ++it) {
		const WordEntries &wordEntries = *it;
		// Debug infos
		//entryCount = wordEntries.mEntrySet.size();
		if (wantTrace) {
			//if (entryCount > 1000) {
				std::string strTmp = wordEntries.ToString();
				Trace("Indexer::StoreWordSetToDb - Db name: %s - %s", inDbName.c_str(), strTmp.c_str());
				//continue;
			//}
			//if (i == 20000)
			//	break;
		}
		dbNdx->AddWordEntries(wordEntries.mWrd, wordEntries.mEntrySet);
		//numWord++;
	}

	// Debug check info only
	//dbNdxCount = dbNdx->Count();

	if (false && wantTrace) {
		Trace("Indexer::StoreWordSetToDb end");
	}
}
//------------------------------------------------------------------------------

void Indexer::StoreWordSetToCache(const std::string &inDbName, const WordEntriesSet *inWordEntriesSet)
{
	bool wantTrace = mStaticTraceStoreDb;

	if (inWordEntriesSet->empty())
		return;

	if (false && wantTrace) {
		size_t setCount = inWordEntriesSet->size();
		Trace("Indexer::StoreWordSetToCache begin (%ld words)", setCount);
	}

	// Get the cache item
	WordEntriesSet *wordEntriesSet;
	WordEntriesSetMap::iterator itFind;
	itFind = mWordEntriesSetMapCache->find(inDbName);
	if (itFind == mWordEntriesSetMapCache->end()) {
		wordEntriesSet = new WordEntriesSet();
		(*mWordEntriesSetMapCache)[inDbName] = wordEntriesSet;
	}
	else
		wordEntriesSet = itFind->second;

	// Add the word entries in the cache
	WordEntriesSet::const_iterator itWes;
	WordEntriesSet::const_iterator it = inWordEntriesSet->begin();
	WordEntriesSet::const_iterator itEnd = inWordEntriesSet->end();
	for (; it != itEnd; ++it) {
		const WordEntries &wordEntries = *it;
		itWes = wordEntriesSet->find(wordEntries);
		if (itWes == wordEntriesSet->end()) {
			WordEntries we;
			we.mWrd = wordEntries.mWrd;
			wordEntriesSet->insert(we);
		}
		itWes = wordEntriesSet->find(wordEntries);
		const WordEntries &weCache = *itWes;
		weCache.AddEntries_ConstHack(wordEntries.mEntrySet);
	}
	/*
	WordEntries *we;
	WordEntriesSet::iterator itWes;
	WordEntriesSet::const_iterator it = inWordEntriesSet->begin();
	WordEntriesSet::const_iterator itEnd = inWordEntriesSet->end();
	for (; it != itEnd; ++it) {
		const WordEntries &wordEntries = *it;
		itWes = wordEntriesSet->find(wordEntries);
		if (itWes == wordEntriesSet->end()) {
			we = new WordEntries();
			we->mWrd = wordEntries.mWrd;
			wordEntriesSet->insert(*we);
		}
		else
			we = &*itWes;
		we->AddEntries(wordEntries.mEntrySet);
	}
	*/

	if (false && wantTrace) {
		Trace("Indexer::StoreWordSetToCache end");
	}
}
//------------------------------------------------------------------------------

void Indexer::StoreNodeToDb(IndexerNode *inIndexerNode)
{
	IndexerItemVector *indexerItemVector = inIndexerNode->mIndexerItemVector;
	if (indexerItemVector == NULL)
		return;

	//gLog.log(eTypLogDebug, "Deb > StoreNodeToDb start: %s", inIndexerNode->ToString().c_str());

	// Iterate on the indexer items
	IndexerItem *indexerItem;
	IndexerItemVector::iterator it, itEnd;
	it = indexerItemVector->begin();
	itEnd = indexerItemVector->end();
	for (; it != itEnd; ++it) {
		indexerItem = *it;
		switch (indexerItem->mTypIndex) {
		case eTypIndexWord:
			{
				// Get the words to index
				WordEntriesSet *wordEntriesSet = indexerItem->GetWordEntriesSetToIndex();
				if (!wordEntriesSet->empty()) {
					// Store into database or in the optional cache
					if (mWordEntriesSetMapCache)
						StoreWordSetToCache(indexerItem->mIndexDbName, wordEntriesSet);
					else
						StoreWordSetToDb(indexerItem->mIndexDbName, wordEntriesSet);
				}
			}
			break;
		case eTypIndexField:
			// Must not index the field until it is closed
			if (inIndexerNode->mIndexerNodeState != IndexerNode::eTypIndexerNodeClose)
				break;
			// Store the field
			if (!indexerItem->mField.isEmpty()) {
				// Get the field to index
				UnicodeString field;
				if (indexerItem->GetFieldToIndex(field)) {
					// Store into database or in the optional cache
					if (mWordEntriesSetMapCache)
						StoreWordEntryToCache(indexerItem->mIndexDbName, field, indexerItem->mEntryStart);
					else
						StoreWordEntryToDb(indexerItem->mIndexDbName, field, indexerItem->mEntryStart);
				}
			}
			break;
		case eTypIndexInterv:
			// Todo: Interval indexation
			break;
		default:
			DEF_Log("Incorrect index type");
			mNbError++;
			break;
		}
	}

	//gLog.log(eTypLogDebug, "Deb > StoreNodeToDb stop");
}
//------------------------------------------------------------------------------

void Indexer::StoreNodeListToDb(bool inFirstList)
{
	size_t count;
	IndexerNode *indexerNode;
	IndexerNodeList *nodeList;
	IndexerNodeList::iterator it, itEnd;

	nodeList = inFirstList ? mIndexerNodeList : mIndexerNodeList2;
	count = nodeList->size();
	if (count == 0)
		return;

	it = nodeList->begin();
	itEnd = nodeList->end();
	for (; it != itEnd; ++it) {
		indexerNode = *it;
		StoreNodeToDb(indexerNode);
		if (!inFirstList)
			delete indexerNode;
	}

	if (!inFirstList)
		nodeList->clear();
}
//------------------------------------------------------------------------------

/*
void Indexer::StoreTokenizerToDb(StringTokenizer *inTokenizer)
{
	// Disable function
	return;

	std::string dbName("fulltext");

	WordEntriesSet *wordEntriesSet = inTokenizer->GetWordEntriesSet();
	if (!wordEntriesSet->empty()) {
		StoreWordSetToDb(dbName, wordEntriesSet);
		wordEntriesSet->clear();
	}
}*/
//------------------------------------------------------------------------------
