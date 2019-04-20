/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Collection.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Collection.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../database/DbDoc.hpp"
#include "../database/DbDocMeta.hpp"
#include "../database/DbFieldMeta.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbMap.hpp"
#include "../database/DbNdx.hpp"
#include "../database/DbSearchRecord.hpp"
//#include "../indexer/Indexer.hpp"
#include "../parser/IndexerHandler.hpp"
#include "../parser/Parser.hpp"
//------------------------------------------------------------------------------

#if defined(WIN32)
	#define DEF_ConfigPath "c:/work/dev/atoll/config"
#else
	#define DEF_ConfigPath "/opt/dev/atoll/config"
#endif
#define DEF_DocMetaDbName "docmeta"
#define DEF_FieldMetaDbName "fieldmeta"
#define DEF_SearchRecordDbName "searchrecord"
#define DEF_DocMapName "_docmap" // Suffix for the collection name
#define DEF_NdxMapName "_ndxmap" // Suffix for the collection name
#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Collection: %s", x); }
//------------------------------------------------------------------------------

class AddDocument;
using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const Collection &inCollection)
{
	inStream << inCollection.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

Collection::Collection(const DbEnv *inEnv, const UnicodeString &inColId)
{
	mDbManager = new DbManager(inEnv, inColId);
	mNdxMap = new UnicodeStringToIntMap;
	mDocMap = new UnicodeStringToIntMap;
	mIntToDocMap = new IntToUnicodeStringMap;

	mNdxObserverSet = new StdStringSet;

	// No indexer parser until there is a document to index
	mIndexerParser = NULL;
	mIndexerHandler = NULL;

	Open();
}
//------------------------------------------------------------------------------

Collection::~Collection()
{
	Close();

	if (mIndexerParser) {
		gLog.log(eTypLogWarning, "Wrn > Indexer not closed");
		CloseIndexer();
	}

	delete mDbManager;
	delete mNdxMap;
	delete mDocMap;
	delete mIntToDocMap;
	delete mNdxObserverSet;
}
//------------------------------------------------------------------------------

const UnicodeString &Collection::GetColId() const
{
	return mDbManager->GetColId();
}
//------------------------------------------------------------------------------

bool Collection::Open()
{
	bool isOk = true;
	std::string mapName;

	// Empty collection name is not allowed
	AppAssert(!mDbManager->GetColId().isEmpty());

	// Load the doc map
	mapName = DEF_DocMapName;
	mDbDocMap = mDbManager->GetDbMap(mapName);
	if (!mDbDocMap->LoadMapFromDb(*mDocMap))
		isOk = false;

	//gLog.log(eTypLogError, "Err > DebugTest Collection::Open: DocMap count=%lu", (unsigned long)mDocMap->size());

	// Create the doc reverse map: number to doc
	UnicodeStringToIntMap::const_iterator it = mDocMap->begin();
	UnicodeStringToIntMap::const_iterator itEnd = mDocMap->end();
	//gLog.log(eTypLogError, "Err > DebugTest Collection::Open: DocMap [Begin]");
	for (; it != itEnd; ++it) {
		const UnicodeString &str = it->first;
		const unsigned int &value = it->second;
		(*mIntToDocMap)[value] = str;
		//gLog.log(eTypLogError, "Err > DebugTest Collection::Open: DocNum=%u", value);
	}
	//gLog.log(eTypLogError, "Err > DebugTest Collection::Open: DocMap [End]");

	// Get the document metadata database
	UnicodeString dbName(DEF_DocMetaDbName);
	mDbDocMeta = mDbManager->GetDbDocMeta(dbName);

	// Get the field metadata database
	dbName = DEF_FieldMetaDbName;
	mDbFieldMeta = mDbManager->GetDbFieldMeta(dbName);

	// Get the search record database
	dbName = DEF_SearchRecordDbName;
	mDbSearchRecord = mDbManager->GetDbSearchRecord(dbName);

	// Load the index map
	mapName = DEF_NdxMapName;
	mDbNdxMap = mDbManager->GetDbMap(mapName);
	if (!mDbNdxMap->LoadMapFromDb(*mNdxMap))
		isOk = false;

	if (!isOk) {
		std::string colId = UnicodeString2String(mDbManager->GetColId());
		gLog.log(eTypLogError, "Err > Collection: Open error for collection: %s", colId.c_str());
	}

	return isOk;
}
//------------------------------------------------------------------------------

void Collection::Close()
{
	// Save the doc map to database
	mDbDocMap->SaveMapToDb(*mDocMap);
	//mDbManager->CloseDbMap(mapName);

	// Save the index map to database
	mDbNdxMap->SaveMapToDb(*mNdxMap);
	//mDbManager->CloseDbMap(mapName);
}
//------------------------------------------------------------------------------

bool Collection::CheckSizes(const char *inLogInfo)
{
	bool isOk = true;

	unsigned long count = (unsigned long)mDocMap->size();
	if (!(mDbDocMap->Count() == count)) {
		gLog.log(eTypLogError, "Err > Collection: Db map size error in '%s' (DbDocMap=%lu != DocMap=%lu)", inLogInfo, mDbDocMap->Count(), count);
		isOk = false;
	}
	if (!((unsigned long)mIntToDocMap->size() == count)) {
		gLog.log(eTypLogError, "Err > Collection: Doc number map size error in '%s' (DbDocMap=%lu != DocMap=%lu)", inLogInfo, mDbDocMap->Count(), count);
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::ClearCollection()
{
	bool isOk = true;

	std::string dbName;
	UnicodeStringToIntMap::const_iterator it;
	UnicodeStringToIntMap::const_iterator itEnd;

	// Remove all the individual doc db
	it = mDocMap->begin();
	itEnd = mDocMap->end();
	for (; it != itEnd; ++it) {
		const unsigned int &value = it->second;
		mDbManager->RemoveDbDoc(value);
	}

	// Clear the document map and it's database storage
	mDocMap->clear();
	mIntToDocMap->clear();
	mDbDocMeta->clear();
	mDbDocMap->clear();

	// Clear the field meta database
	mDbFieldMeta->clear();

	// Clear the search record database
	mDbSearchRecord->clear();

	// Remove all individual index db
	it = mNdxMap->begin();
	itEnd = mNdxMap->end();
	for (; it != itEnd; ++it) {
		const UnicodeString &key = it->first;
		ConvertUnicodeString2String(dbName, key);
		mDbManager->RemoveDbNdx(dbName);
	}

	// Clear the index map and it's database storage
	mNdxMap->clear();
	mDbNdxMap->clear();

	// Check the container sizes
	isOk = CheckSizes("ClearCollection");

	return isOk;
}
//------------------------------------------------------------------------------

void Collection::OpenIndexer(const std::string &inIndexerConfigFileName,
														 const std::string &inRecordBreakerConfigFileName,
														 bool inValidate, bool inDbDocStorage)
{
	AppAssert(mIndexerParser == NULL);

	// Create the parser
	SAX2XMLReader::ValSchemes valScheme = inValidate ? SAX2XMLReader::Val_Auto : SAX2XMLReader::Val_Never;
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	mIndexerParser = new Parser(valScheme, catalogFile, eTypHandlerIndexer);

	// Initialize the indexer
	mIndexerHandler = static_cast<IndexerHandler *>(mIndexerParser->GetHandler());
	mIndexerHandler->SetDbManager(mDbManager);
	bool isIndexerConfig = !inIndexerConfigFileName.empty();
	if (isIndexerConfig)
		mIndexerHandler->LoadIndexerConfig(inIndexerConfigFileName);
	else
		gLog.log(eTypLogWarning, "Wrn > OpenIndexer: Empty indexer config file name");
	bool isRecordBreaker = !inRecordBreakerConfigFileName.empty();
	if (isRecordBreaker)
		mIndexerHandler->LoadRecordBreakerConfig(inRecordBreakerConfigFileName);
	mIndexerHandler->EnableRecordBreaker(isRecordBreaker);
	mIndexerHandler->EnableDbDocStorage(inDbDocStorage);

	// Set the index database observer
	// This oberserver stores all the opened index databases
	mNdxObserverSet->clear();
	mDbManager->SetNdxObserver(mNdxObserverSet);
}
//------------------------------------------------------------------------------

void Collection::CloseIndexer()
{
	if (mIndexerParser == NULL) {
		gLog.log(eTypLogWarning, "Wrn > Indexer already closed");
		return;
	}

	// Store the name and size of all the opened index databases
	StdStringSet::const_iterator it = mNdxObserverSet->begin();
	StdStringSet::const_iterator itEnd = mNdxObserverSet->end();
	for (; it != itEnd; ++it) {
		const std::string &dbName = *it;
		AddIndexDatabase(dbName);
	}

	// Remove the index database observer
	mDbManager->SetNdxObserver(NULL);

	// Disable the record breaker
	mIndexerHandler->EnableRecordBreaker(false);

	// Enable the document storage
	mIndexerHandler->EnableDbDocStorage(true);

	// Raz indexer handler
	mIndexerHandler = NULL; // Memory is hold by the parser

	// Delete the indexer parser
	delete mIndexerParser;
	mIndexerParser = NULL;
}
//------------------------------------------------------------------------------

bool Collection::AddDocument(unsigned int &outDocNum, const DocMeta &inDocMeta, bool inWantIndex /*= true*/)
{
	bool isOk = true;

	outDocNum = 0;

	// Check if the file exists
	if (!FileSystem::FileExists(inDocMeta.mFileName)) {
		gLog.log(eTypLogError, "Err > AddDocument: Document file doesn't exists: %s", inDocMeta.mFileName.c_str());
		return false;
	}

	// Search in the doc map if the doc already exists
	UnicodeString fileName;
	ConvertString2UnicodeString(fileName, inDocMeta.mFileName);
	UnicodeStringToIntMap::const_iterator it1 = mDocMap->find(fileName);
	if (it1 != mDocMap->end()) {
		const unsigned int &value = it1->second;
		gLog.log(eTypLogError, "Err > AddDocument: Try to add an already existing doc (docnum=%u): %s", value, inDocMeta.mFileName.c_str());
		return false;
	}

	// Search the first doc number available (greatest + 1)
	unsigned int docNum = 1; // Min doc number is 1
	UnicodeStringToIntMap::const_iterator it = mDocMap->begin();
	UnicodeStringToIntMap::const_iterator itEnd = mDocMap->end();
	for (; it != itEnd; ++it) {
		const unsigned int &value = it->second;
		if (value >= docNum)
			docNum = value + 1;
	}

	//gLog.log(eTypLogError, "Err > DebugTest AddDocument: Uuid=%s DocNum=%u in DocMap count=%lu", inDocMeta.mFileName.c_str(), docNum, (unsigned long)mDocMap->size());

	// Add the document metadata record in the database
	DocMeta docMeta;
	docMeta = inDocMeta;
	docMeta.mDocNum = docNum;
	docMeta.mCountPge = 0; // Cannot be set as the document is new
	docMeta.mIsIndexed = false; // Cannot be set as the document is new
	// Avoid empty doc title
	if (UnicodeStringIsOnlySpace(docMeta.mDocTitle.getTerminatedBuffer())) {
		gLog.log(eTypLogError, "Err > AddDocument: Empty doc title");
		docMeta.mDocTitle = "No title";
	}
	// Avoid empty uuid
	if (UnicodeStringIsOnlySpace(docMeta.mUuid.getTerminatedBuffer())) {
		std::string fileName = FileSystem::GetFileNameWithoutExt(docMeta.mFileName);
		ConvertString2UnicodeString(docMeta.mUuid, fileName);
	}
	if (!mDbDocMeta->AddDocMeta(docMeta))
		return false;

	// Store doc file name and number in the map database
	mDbDocMap->AddElement(fileName, docNum);

	// Add the document in the doc maps
	(*mDocMap)[fileName] = docNum;
	(*mIntToDocMap)[docNum] = fileName;

	// Returns the doc num
	outDocNum = docNum;

	// Check the container sizes
	if (!CheckSizes("AddDocument"))
		return false;

	if (inWantIndex) {
		isOk &= IndexDocument(docNum);
	}

	//gLog.log(eTypLogError, "Err > DebugTest AddDocument: Uuid=%s DocNum=%u in DocMap count=%lu", inDocMeta.mFileName.c_str(), docNum, (unsigned long)mDocMap->size());

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::RemoveDocument(unsigned int inDocNum)
{
	bool isOk = true;

	// Remove the document index and it's database storage
	if (!RemoveDocumentIndex(inDocNum))
		return false;

	// Remove the doc in the document maps
	IntToUnicodeStringMap::iterator it2 = mIntToDocMap->find(inDocNum); // Cannot be const_iterator
	if (it2 == mIntToDocMap->end()) {
		gLog.log(eTypLogError, "Err > RemoveDocument: Unknown document: %u", inDocNum);
		return false;
	}
	const UnicodeString fileName = it2->second;
	UnicodeStringToIntMap::iterator it1 = mDocMap->find(fileName); // Cannot be const_iterator
	if (it1 == mDocMap->end()) {
		std::string docFileName = UnicodeString2String(fileName);
		gLog.log(eTypLogError, "Err > RemoveDocument: Unknown document: %u: %s", docFileName.c_str());
		return false;
	}
	mDocMap->erase(it1);
	mIntToDocMap->erase(it2);

	// Remove the document metadata record
	isOk &= mDbDocMeta->DelDocMeta(inDocNum);

	// Remove document from the map database
	mDbDocMap->DelElement(fileName);

	// Check the container sizes
	isOk &= CheckSizes("RemoveDocument");

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::DocumentExist(unsigned int inDocNum)
{
	IntToUnicodeStringMap::const_iterator it = mIntToDocMap->find(inDocNum);
	if (it == mIntToDocMap->end()) {
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool Collection::GetDocumentMeta(DocMeta &ioDocMeta)
{
	// Set docnum if not given
	if (ioDocMeta.mDocNum == 0) {
		short method = 0;
		if (!ioDocMeta.mUuid.isEmpty()) {
			method = 1;
		}
		// Get the vector of all documents metadata records
		DocMetaVector vector;
		bool isOk = GetDocMetaVector(vector);
		// Iterate the vector test documents metadata
		if (isOk) {
			DocMetaVector::const_iterator it = vector.begin();
			DocMetaVector::const_iterator itEnd = vector.end();
			for (; it != itEnd; ++it) {
				const DocMeta &docMeta = (*it);
				switch (method) {
				case 1:
					if (docMeta.mUuid == ioDocMeta.mUuid) {
						ioDocMeta.mDocNum = docMeta.mDocNum;
					}
					break;
				}
				// Quit if docnum is found
				if (ioDocMeta.mDocNum)
					break;
			}
		}
	}

	// Search in the doc map if the document exists
	if (!DocumentExist(ioDocMeta.mDocNum)) {
		gLog.log(eTypLogError, "Err > GetDocumentMeta: Unknown document: %u", ioDocMeta.mDocNum);
		return false;
	}

	// Get the record
	bool isOk = mDbDocMeta->GetDocMeta(ioDocMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::SetDocumentMeta(const DocMeta &inDocMeta)
{
	bool isOk = true;
	//bool wantIndex = false;
	DocMeta docMeta(inDocMeta.mDocNum);

	// Search in the doc map if the document exists
	if (!DocumentExist(inDocMeta.mDocNum)) {
		gLog.log(eTypLogError, "Err > SetDocumentMeta: Unknown document: %u", inDocMeta.mDocNum);
		return false;
	}

	// Check if the file exists
	if (!FileSystem::FileExists(inDocMeta.mFileName)) {
		gLog.log(eTypLogError, "Err > SetDocumentMeta: Document file doesn't exists: %s", inDocMeta.mFileName.c_str());
		return false;
	}

	// Get the previous record if any
	mDbDocMeta->GetDocMeta(docMeta);

	// Change the document storage if the document has changed
	if (!(inDocMeta.mFileName == docMeta.mFileName)) {
		// Search the old doc in the map (it must exist)
		UnicodeString oldFileName;
		ConvertString2UnicodeString(oldFileName, docMeta.mFileName);
		UnicodeStringToIntMap::iterator it1 = mDocMap->find(oldFileName); // Cannot be const_iterator
		if (it1 == mDocMap->end()) {
			gLog.log(eTypLogError, "Err > SetDocumentMeta: Unknown document: %u: %s", docMeta.mFileName.c_str());
			return false;
		}
		// Search the new doc in the map (it must not exists)
		UnicodeString fileName;
		ConvertString2UnicodeString(fileName, inDocMeta.mFileName);
		UnicodeStringToIntMap::const_iterator it2 = mDocMap->find(fileName);
		if (it2 != mDocMap->end()) {
			gLog.log(eTypLogError, "Err > SetDocumentMeta: Try to set an already existing doc: %s", inDocMeta.mFileName.c_str());
			return false;
		}
		// Remove the document index and it's database storage if it's already indexed
		if (docMeta.mIsIndexed) {
			//wantIndex = true;
			if (!RemoveDocumentIndex(docMeta.mDocNum))
				return false;
			docMeta.mCountPge = 0;
			docMeta.mIsIndexed = false;
		}
		// Replace the document
		docMeta.mFileName = inDocMeta.mFileName;
		mDocMap->erase(it1);
		(*mDocMap)[fileName] = docMeta.mDocNum;
		(*mIntToDocMap)[docMeta.mDocNum] = fileName;
		mDbDocMap->DelElement(oldFileName);
		mDbDocMap->AddElement(fileName, docMeta.mDocNum);
		// Check the container sizes
		isOk &= CheckSizes("SetDocumentMeta");
	}

	// Set new meta (not the reserved fields mDocNum, mCountPge, mFileName and mIsIndexed)
	docMeta.mXmlValidation = inDocMeta.mXmlValidation;
	docMeta.mDocTitle = inDocMeta.mDocTitle;
	if (UnicodeStringIsOnlySpace(docMeta.mDocTitle.getTerminatedBuffer())) {
		gLog.log(eTypLogError, "Err > SetDocumentMeta: Empty doc title");
		docMeta.mDocTitle = "No title";
	}
	docMeta.mSource = inDocMeta.mSource;
	docMeta.mUuid = inDocMeta.mUuid;
	docMeta.mXsltCode = inDocMeta.mXsltCode;
	docMeta.mDcMetadata = inDocMeta.mDcMetadata;
	docMeta.mIndexerConfig = inDocMeta.mIndexerConfig;
	docMeta.mRecordBreakerConfig = inDocMeta.mRecordBreakerConfig;

	// Set or add the record
	isOk &= mDbDocMeta->AddDocMeta(docMeta);

	// Index the document if it's content has changed and it was previously indexed
	// This is not mandatory: We can also let the document without index
	//if (wantIndex)
	//	isOk &= IndexDocument(docMeta.mDocNum);

	//if (docMeta.mIsIndexed)
	//	isOk &= IndexDocMeta(docMeta, false);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::IndexDocument(unsigned int inDocNum)
{
	bool isOk = true;

	// Search in the doc map if the doc exists
	if (!DocumentExist(inDocNum)) {
		gLog.log(eTypLogError, "Err > IndexDocument: Unknown document: %u", inDocNum);
		return false;
	}

	// Get the document metadata
	DocMeta docMeta(inDocNum);
	if (!mDbDocMeta->GetDocMeta(docMeta))
		return false;

	// Remove the document index and it's database storage if it's already indexed
	if (docMeta.mIsIndexed) {
		if (!RemoveDocumentIndex(inDocNum))
			return false;
	}

	// Index the document
	bool dbDocStorage = true;
	bool traceErrorBak, traceError = false; // Disable xerces errors
	OpenIndexer(docMeta.mIndexerConfig, docMeta.mRecordBreakerConfig, docMeta.mXmlValidation, dbDocStorage);
	mIndexerHandler->SetDocRef(inDocNum);
	traceErrorBak = mIndexerHandler->GetTraceError();
	mIndexerHandler->SetTraceError(traceError);
	isOk = mIndexerParser->XercesParse(docMeta.mFileName);
	mIndexerHandler->SetTraceError(traceErrorBak);
	CloseIndexer();

	// Update the document metadata
	if (isOk) {
		docMeta.mIsIndexed = true;
		DbDoc *dbDoc = GetDocumentDatabase(inDocNum);
		if (dbDoc)
			docMeta.mCountPge = dbDoc->Count();
		isOk &= mDbDocMeta->AddDocMeta(docMeta);
		isOk &= IndexDocMeta(docMeta);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::IndexDocMeta(const DocMeta &inDocMeta)
{
	bool isOk = true;

	// Search in the doc map if the doc exists
	if (!DocumentExist(inDocMeta.mDocNum)) {
		gLog.log(eTypLogError, "Err > IndexDocMeta: Unknown document: %u", inDocMeta.mDocNum);
		return false;
	}

	// Index the document metadata
	// but don't store the record into database
	bool validate = true;
	bool dbDocStorage = false; // This will also index all metadata on the document first position
	std::string indexerConfigFileName, recordBreakerConfigFileName;
	indexerConfigFileName = DEF_ConfigPath + std::string("/atoll_indexer_config_metadata.xml");
	// Index the document metadata
	{
		UnicodeString strDocMeta;
		inDocMeta.ToXml(strDocMeta);
		OpenIndexer(indexerConfigFileName, recordBreakerConfigFileName, validate, dbDocStorage);
		mIndexerHandler->SetDocRef(inDocMeta.mDocNum);
		isOk = mIndexerParser->XercesParse(strDocMeta.getTerminatedBuffer());
		CloseIndexer();
	}
	// Index the dublin core document metadata file if any
	if (isOk && !inDocMeta.mDcMetadata.empty()) {
		OpenIndexer(indexerConfigFileName, recordBreakerConfigFileName, validate, dbDocStorage);
		mIndexerHandler->SetDocRef(inDocMeta.mDocNum);
		isOk = mIndexerParser->XercesParse(inDocMeta.mDcMetadata);
		CloseIndexer();
	}

	// Index the document metadata
	/* This is the old manual method
	Entry entry(inDocMeta.mDocNum, 0, 1); // On the document first position
	DbNdx *dbNdx;
	std::string dbName;
	dbName = "metadata_title";
	dbNdx = mDbManager->GetDbNdx(dbName);
	dbNdx->DelDocEntries(inDocMeta.mDocNum);
	UnicodeString docTitle = inDocMeta.mDocTitle;
	if (UnicodeStringIsOnlySpace(docTitle.getTerminatedBuffer())) {
		gLog.log(eTypLogError, "Err > IndexDocMeta: Empty doc title");
		docTitle = "No title";
	}
	dbNdx->AddWordEntry(docTitle, entry);
	AddIndexDatabase(dbName);
	*/

	// Index the document metadata
	/*
	std::string elem;
	StringToUnicodeStringMap attrMap;
	WordEntry wordEntry;
	Indexer *indexer = new Indexer;
	indexer->LoadIndexerConfig(inDocMeta.mIndexerConfig);
	//indexer->StartNode(mElem, mAttrMap);
	//indexer->EndNode();
	wordEntry
	indexer->Indexer->IndexerAddWordEntry(*mWe);
	indexer->StoreNodeListToDb(false);
	delete indexer;
	*/

	/*
	// Get the vector of nodes to index, or NULL if there is nothing to index
	std::string elem;
	StringToUnicodeStringMap attrMap;
	IndexerItemVector *indexerItemVector = mIndexerConfig->GetNodeIndexerVector(elem, attrMap);
	//indexer->StartNode(mElem, mAttrMap);
	// Create an indexer for the node
	IndexerNode *indexerNode = new IndexerNode(indexerItemVector);
	indexer->StoreNodeToDb(IndexerNode *inIndexerNode);
	*/

	/*
	mIndexerHandler->InitHandler();
	mHandler->BeginDocument();

	std::string inElem;
	StringToUnicodeStringMap inAttrMap;
	// Get the vector of nodes to index, or NULL if there is nothing to index
	IndexerItemVector *indexerItemVector = mIndexerConfig->GetNodeIndexerVector(inElem, inAttrMap);
	// Create an indexer for the node
	IndexerNode *indexerNode = new IndexerNode(indexerItemVector);
	// Add the node in the list
	//mIndexerNodeList->push_back(indexerNode);
	indexerNode->InAddWordEntry(inWordEntry); // WordEntry &inWordEntry
	//StoreNodeToDb(IndexerNode *inIndexerNode);

	mHandler->EndDocument(!isOk);
	unsigned long nbErrFile = mHandler->GetNbError();
	if (nbErrFile)
		isOk = false;
	CloseIndexer();
	*/

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::RemoveDocumentIndex(unsigned int inDocNum)
{
	// Search in the doc map if the doc exists
	if (!DocumentExist(inDocNum)) {
		gLog.log(eTypLogError, "Err > RemoveDocumentIndex: Unknown document: %u", inDocNum);
		return false;
	}

	// Get the document metadata
	DocMeta docMeta(inDocNum);
	if (!mDbDocMeta->GetDocMeta(docMeta))
		return false;

	// Nothing to do if the document is not indexed
	if (!docMeta.mIsIndexed)
		return true;

	// Remove the doc database
	mDbManager->RemoveDbDoc(inDocNum);

	// Remove the doc in the index databases
	DbNdx *dbNdx;
	std::string dbName;
	unsigned long count;
	UnicodeStringToIntMap::iterator it, itBak;
	UnicodeStringToIntMap::const_iterator itEnd;
	it = mNdxMap->begin();
	itEnd = mNdxMap->end();
	for (; it != itEnd;) {
		const UnicodeString &key = it->first;
		ConvertUnicodeString2String(dbName, key);
		dbNdx = mDbManager->GetDbNdx(dbName);
		dbNdx->DelDocEntries(inDocNum);
		count = dbNdx->Count();
		// Remove the ndx database if empty
		if (!count) {
			mDbManager->RemoveDbNdx(dbName);
			itBak = it++;
			mNdxMap->erase(itBak);
		}
		else {
			it->second = count;
			it++;
		}
	}

	// Update the document metadata
	docMeta.mIsIndexed = false;
	docMeta.mCountPge = 0;
	bool isOk = mDbDocMeta->AddDocMeta(docMeta);

	return isOk; // true
}
//------------------------------------------------------------------------------

unsigned long Collection::GetDocumentCount()
{
	unsigned long size = (unsigned long)mDocMap->size();

	return size;
}
//------------------------------------------------------------------------------

bool Collection::GetDocMetaVector(DocMetaVector &outVector)
{
	bool isOk = mDbDocMeta->GetDocMetaVector(outVector);

	return isOk;
}
//------------------------------------------------------------------------------

DbDoc *Collection::GetDocumentDatabase(unsigned int inDocNum)
{
	// Search in the doc map if the doc exists
	if (!DocumentExist(inDocNum)) {
		gLog.log(eTypLogError, "Err > GetDocumentDatabase: Unknown document: %u", inDocNum);
		return NULL;
	}

	// Return the doc database
	return mDbManager->GetDbDoc(inDocNum);
}
//------------------------------------------------------------------------------

bool Collection::GetFieldMetaVector(FieldMetaVector &outVector, eTypField inTypField)
{
	bool isOk = mDbFieldMeta->GetFieldMetaVector(outVector, inTypField);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::AddFieldMeta(const FieldMeta &inFieldMeta)
{
	bool isOk = mDbFieldMeta->AddFieldMeta(inFieldMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::GetFieldMeta(FieldMeta &ioFieldMeta)
{
	// Get the record
	bool isOk = mDbFieldMeta->GetFieldMeta(ioFieldMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::SetFieldMeta(const FieldMeta &inFieldMeta)
{
	// Set or add the record
	bool isOk = mDbFieldMeta->AddFieldMeta(inFieldMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::RemoveFieldMeta(const FieldMeta &inFieldMeta)
{
	// Remove the record
	bool isOk = mDbFieldMeta->DelFieldMeta(inFieldMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::GetSearchRecordVector(SearchRecordVector &outVector, eTypSearchRecord inTypSearchRecord)
{
	bool isOk = mDbSearchRecord->GetSearchRecordVector(outVector, inTypSearchRecord);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::AddSearchRecord(const SearchRecord &inSearchRecord)
{
	bool isOk = mDbSearchRecord->AddSearchRecord(inSearchRecord);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::GetSearchRecord(SearchRecord &ioSearchRecord)
{
	// Get the record
	bool isOk = mDbSearchRecord->GetSearchRecord(ioSearchRecord);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::SetSearchRecord(const SearchRecord &inSearchRecord)
{
	// Set or add the record
	bool isOk = mDbSearchRecord->AddSearchRecord(inSearchRecord);

	return isOk;
}
//------------------------------------------------------------------------------

bool Collection::RemoveSearchRecord(const SearchRecord &inSearchRecord)
{
	// Remove the record
	bool isOk = mDbSearchRecord->DelSearchRecord(inSearchRecord);

	return isOk;
}
//------------------------------------------------------------------------------

unsigned long Collection::GetIndexCount()
{
	unsigned long size = (unsigned long)mNdxMap->size();

	return size;
}
//------------------------------------------------------------------------------

bool Collection::GetIndexMap(UnicodeStringToIntMap &outMap)
{
	outMap = *mNdxMap;

	return true;
}
//------------------------------------------------------------------------------

unsigned int Collection::GetIndexNumber(const std::string &inNdxName)
{
	UnicodeString ndxName;
	ConvertString2UnicodeString(ndxName, inNdxName);
	UnicodeStringToIntMap::const_iterator it = mNdxMap->find(ndxName);
	if (it == mNdxMap->end()) {
		return DEF_IntMax;
	}

	return it->second;
}
//------------------------------------------------------------------------------

DbNdx *Collection::GetIndexDatabase(const std::string &inNdxName)
{
	// Search in the index map if the index exists
	unsigned int ndxNum = GetIndexNumber(inNdxName);
	if (ndxNum == DEF_IntMax) {
		//gLog.log(eTypLogError, "Err > GetIndexDatabase: Unknown index: %s", inNdxName.c_str());
		return NULL;
	}

	// Return the index database
	return mDbManager->GetDbNdx(inNdxName);
}
//------------------------------------------------------------------------------

void Collection::AddIndexDatabase(const std::string &inDbName, bool inLogIfError /*= true*/)
{
	DbNdx *dbNdx;
	UnicodeString key;
	unsigned long count;
	UnicodeStringToIntMap::iterator itMap;
	UnicodeStringToIntMap::const_iterator itMapEnd = mNdxMap->end();

	dbNdx = mDbManager->GetDbNdx(inDbName);
	count = dbNdx->Count();
	if (!count && inLogIfError)
		gLog.log(eTypLogError, "Err > AddIndexDatabase: Empty ndx db '%s'", inDbName.c_str());
	// Update db info
	ConvertString2UnicodeString(key, inDbName);
	itMap = mNdxMap->find(key);
	if (itMap == itMapEnd) {
		(*mNdxMap)[key] = count; // Add the index database with it's count
	}
	else
		itMap->second = count;
}
//------------------------------------------------------------------------------

std::string Collection::ToString() const
{
	std::string s("Collection: ");

	std::string colId = UnicodeString2String(GetColId());
	s += colId.c_str();

	return s;
}
//------------------------------------------------------------------------------
