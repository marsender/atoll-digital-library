/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Collection.hpp

*******************************************************************************/

#ifndef __Collection_HPP
#define __Collection_HPP
//------------------------------------------------------------------------------

#include "../common/Containers.hpp"
#include "../util/DocMeta.hpp"
#include "../util/FieldMeta.hpp"
#include "../util/SearchRecord.hpp"
//------------------------------------------------------------------------------

class DbEnv; // No namespace

namespace Atoll
{
class DbDoc;
class DbDocMeta;
class DbFieldMeta;
class DbSearchRecord;
class DbMap;
class DbNdx;
class DbManager;
class Parser;
class IndexerHandler;

//! Application object that tight together documents, index and fields
/**
	Logic:
		- Create a database manager to centralise all access
		- Holds the list of all the documents that can be accessed
		- Give acces to all the search index in the database
		- Give acces to the fields in the database
*/
class DEF_Export Collection
{
public:
	Collection(const DbEnv *inEnv, const UnicodeString &inColId);
	~Collection();

	//! Get the collection name
	const UnicodeString &GetColId() const;

	//! Remove all stored documents, index and fields
	bool ClearCollection();

	//! Check if a document exist
	bool DocumentExist(unsigned int inDocNum);
	//! Add a document. The document file name must be set. Returns the new document number
	bool AddDocument(unsigned int &outDocNum, const DocMeta &inDocMeta, bool inWantIndex = true);
	//! Remove a document
	bool RemoveDocument(unsigned int inDocNum);
	//! Get the metadata record for a document. The metadata field document number must be set.
	bool GetDocumentMeta(DocMeta &outDocMeta);
	//! Set a document metadata record
	bool SetDocumentMeta(const DocMeta &inDocMeta);

	//! Index a document
	bool IndexDocument(unsigned int inDocNum);
	//! Remove a document index and it's database storage
	bool RemoveDocumentIndex(unsigned int inDocNum);

	//! Get the number of documents in the database
	unsigned long GetDocumentCount();
	//! Get the list of all document metadata records
	bool GetDocMetaVector(DocMetaVector &outVector);
	//! Get a document database from the doc num
	DbDoc *GetDocumentDatabase(unsigned int inDocNum);

	//! Get the list of all field metadata records
	bool GetFieldMetaVector(FieldMetaVector &outVector, eTypField inTypField);
	//! Add a field metadata
	bool AddFieldMeta(const FieldMeta &inFieldMeta);
	//! Get the metadata record for a field. The metadata field type and id number must be set.
	bool GetFieldMeta(FieldMeta &ioFieldMeta);
	//! Set a field metadata record
	bool SetFieldMeta(const FieldMeta &inFieldMeta);
	//! Remove a field metadata record
	bool RemoveFieldMeta(const FieldMeta &inFieldMeta);

	//! Get the list of all search records
	bool GetSearchRecordVector(SearchRecordVector &outVector, eTypSearchRecord inTypSearchRecord);
	//! Add a search record
	bool AddSearchRecord(const SearchRecord &inSearchRecord);
	//! Get the metadata record for a search. The search record type and id must be set.
	bool GetSearchRecord(SearchRecord &ioSearchRecord);
	//! Set a search record
	bool SetSearchRecord(const SearchRecord &inSearchRecord);
	//! Remove a search record
	bool RemoveSearchRecord(const SearchRecord &inSearchRecord);

	//! Get the number of index in the database
	unsigned long GetIndexCount();
	//! Get the index map
	bool GetIndexMap(Common::UnicodeStringToIntMap &outMap);
	//! Get an index number from it's name (DEF_ShortMax if no index)
	unsigned int GetIndexNumber(const std::string &inNdxName);
	//! Get an index database, or NULL if the database doesn't exist
	DbNdx *GetIndexDatabase(const std::string &inNdxName);
	//! Add an index database
	void AddIndexDatabase(const std::string &inDbName, bool inLogIfError = true);

	std::string ToString() const;

private:
	DbManager *mDbManager; //!< Unique access to all databases
	Common::UnicodeStringToIntMap *mDocMap; //!< Map of all existing document for this collection
	Common::IntToUnicodeStringMap *mIntToDocMap; //!< Reverse map of all existing document for this collection
	Common::UnicodeStringToIntMap *mNdxMap; //!< Map of all existing index for this collection
	Parser *mIndexerParser; //!< Xerces document indexer parser
	IndexerHandler *mIndexerHandler; //!< Document indexer
	Common::StdStringSet *mNdxObserverSet; //!< Observer for opened index databases
	DbDocMeta *mDbDocMeta; //!< Database of document metadata
	DbFieldMeta *mDbFieldMeta; //!< Database of field metadata
	DbSearchRecord *mDbSearchRecord; //!< Database of search records
	DbMap *mDbDocMap; //!< Database of the document map
	DbMap *mDbNdxMap; //!< Database of the index map

	//! Open the collection
	bool OpenCollection();
	//! Close the collection
	void CloseCollection();

	//! Check the container sizes
	bool CheckSizes();

	//! Open the document indexer
	void OpenIndexer(const std::string &inIndexerConfigFileName,
		const std::string &inRecordBreakerConfigFileName, bool inValidate,
		bool inDbDocStorage);
	//! Close the document indexer
	void CloseIndexer();
	//! Index the document metadata
	bool IndexDocMeta(const DocMeta &inDocMeta);
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const Collection &inCollection);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
