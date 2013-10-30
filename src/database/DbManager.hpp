/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbManager.hpp

*******************************************************************************/

#ifndef __DbManager_HPP
#define __DbManager_HPP

#include "../common/Containers.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

class DbEnv; // No namespace

namespace Atoll
{
class DbBlob;
class DbColMeta;
class DbDocMeta;
class DbFieldMeta;
class DbSearchRecord;
class DbDoc;
class DbNdx;
class DbMap;

//! Database manager. Holds database tables in a specific environment
/**
	Logic:
		- The database is in the directory specified in the environment
		- The database tables are accessed by name
		- Each opened database table stays opened by default, but can be closed manualy
		- Opened database tables are stored in a map for fast access
		- Types of database tables:
			* DbBlob: key to blob container
			* DbColMeta: collection metadata container
			* DbDocMeta: document metadata container
			* DbFieldMeta: field metadata container
			* DbSearchRecord: search record container
			* DbDoc: document container
			* DbNdx: words sorted alphabeticaly, using unicode collation
			* DbMap: unicode string to binary buffer map
*/
class DbManager
{
public:
	//! Open database in specific environment
	DbManager(const DbEnv *inDbEnv, const UnicodeString &inColId);
	~DbManager();

	// Get the database environment
	DbEnv *GetDbEnv() { return mDbEnv; }

	//! Get the collection name
	const UnicodeString &GetColId() const;

	//! Get the full database name, including the collection name
	std::string GetFullDbName(unsigned int inDbName) const;
	std::string GetFullDbName(const std::string &inDbName) const;
	std::string GetFullDbName(const UnicodeString &inDbName) const;

	// Get the database home directory
	//const std::string &GetHome() const { return mDbHome; }

	//! Close all databases
	void CloseAllDb();
	//! Close all blob databases
	void CloseAllBlobDb();
	//! Close all collection metadata databases
	void CloseAllColMetaDb();
	//! Close all document metadata databases
	void CloseAllDocMetaDb();
	//! Close all field metadata databases
	void CloseAllFieldMetaDb();
	//! Close all search record databases
	void CloseAllSearchRecordDb();
	//! Close all document databases
	void CloseAllDocDb();
	//! Close all index databases
	void CloseAllNdxDb();
	//! Close all map databases
	void CloseAllMapDb();

	//! Remove a database (doesn't work: use DbWrapper::remove() instead)
	//void RemoveDb(const std::string &inDbName);

	//! Get a collection metadata database, specified by it's name (open it if necessary)
	DbColMeta *GetDbColMeta(const UnicodeString &inDbName);
	//! Close a collection metadata database
	void CloseDbColMeta(const UnicodeString &inDbName);
	//! Remove a collection metadata database
	void RemoveDbColMeta(const UnicodeString &inDbName);

	//! Get a document metadata database, specified by it's name (open it if necessary)
	DbDocMeta *GetDbDocMeta(const UnicodeString &inDbName);
	//! Close a document metadata database
	void CloseDbDocMeta(const UnicodeString &inDbName);
	//! Remove a document metadata database
	void RemoveDbDocMeta(const UnicodeString &inDbName);

	//! Get a field metadata database, specified by it's name (open it if necessary)
	DbFieldMeta *GetDbFieldMeta(const UnicodeString &inDbName);
	//! Close a field metadata database
	void CloseDbFieldMeta(const UnicodeString &inDbName);
	//! Remove a field metadata database
	void RemoveDbFieldMeta(const UnicodeString &inDbName);

	//! Get a search record database, specified by it's name (open it if necessary)
	DbSearchRecord *GetDbSearchRecord(const UnicodeString &inDbName);
	//! Close a search record database
	void CloseDbSearchRecord(const UnicodeString &inDbName);
	//! Remove a search record database
	void RemoveDbSearchRecord(const UnicodeString &inDbName);

	//! Get a document database, specified by it's name (open it if necessary)
	DbDoc *GetDbDoc(unsigned int inDocNum);
	//! Close a document database
	void CloseDbDoc(unsigned int inDocNum);
	//! Remove a document database
	void RemoveDbDoc(unsigned int inDocNum);

	//! Get an index database, specified by it's name (open it if necessary)
	DbNdx *GetDbNdx(const std::string &inDbName);
	//! Close an index database
	void CloseDbNdx(const std::string &inDbName);
	//! Remove an index database
	void RemoveDbNdx(const std::string &inDbName);
	//! Set the observer string set witch stores the name of all the opened index databases
	void SetNdxObserver(Common::StdStringSet *inStringSet);

	//! Get a map database, specified by it's name (open it if necessary)
	DbMap *GetDbMap(const std::string &inDbName);
	//! Close a map database
	void CloseDbMap(const std::string &inDbName);

	//! Get a blob database, specified by it's name (open it if necessary)
	DbBlob *GetDbBlob(const std::string &inDbName);
	//! Close a blob database
	void CloseDbBlob(const std::string &inDbName);

	//bool isTransactedEnv() const { return ((mDbEnvOpenFlags & DB_INIT_TXN) != 0); }
	//bool isLockingEnv() const { return ((mDbEnvOpenFlags & DB_INIT_LOCK) != 0); }
	//bool isCDBEnv() const { return ((mDbEnvOpenFlags & DB_INIT_CDB) != 0); }

	//! Clear databases when opening them
	void SetClearOnOpen(bool inClearOnOpen) { mClearOnOpen = inClearOnOpen; }
	//! Remove databases when closing them
	void SetRemoveOnClose(bool inRemoveOnClose) { mRemoveOnClose = inRemoveOnClose; }

private:
	typedef std::map <UnicodeString, DbColMeta *> DbColMetaMap;
	typedef std::map <UnicodeString, DbDocMeta *> DbDocMetaMap;
	typedef std::map <UnicodeString, DbFieldMeta *> DbFieldMetaMap;
	typedef std::map <UnicodeString, DbSearchRecord *> DbSearchRecordMap;
	typedef std::map <unsigned int, DbDoc *> DbDocMap;
	typedef std::map <std::string, DbNdx *> DbNdxMap;
	typedef std::map <std::string, DbMap *> DbMapMap;
	typedef std::map <std::string, DbBlob *> DbBlobMap;

	DbEnv *mDbEnv;
	UnicodeString mColId; //!< Code = unique id of the collection
	//u_int32_t mDbEnvOpenFlags;
	//std::string mDbHome;
	DbColMetaMap mDbColMetaMap;
	DbColMeta *mDbColMetaCurrent;
	DbDocMetaMap mDbDocMetaMap;
	DbDocMeta *mDbDocMetaCurrent;
	DbFieldMetaMap mDbFieldMetaMap;
	DbFieldMeta *mDbFieldMetaCurrent;
	DbSearchRecordMap mDbSearchRecordMap;
	DbSearchRecord *mDbSearchRecordCurrent;
	DbDocMap mDbDocMap;
	DbDoc *mDbDocCurrent;
	DbNdxMap mDbNdxMap;
	DbNdx *mDbNdxCurrent;
	DbMapMap mDbMapMap;
	DbMap *mDbMapCurrent;
	DbBlobMap mDbBlobMap;
	DbBlob *mDbBlobCurrent;
	Common::StdStringSet *mNdxObserverSet; //!< Observer for opened index databases
	bool mClearOnOpen, mRemoveOnClose;

	//! Open a collection metadata database
	DbColMeta *OpenDbColMeta(const std::string &inDbName);
	//! Open a document metadata database
	DbDocMeta *OpenDbDocMeta(const std::string &inDbName);
	//! Open a field metadata database
	DbFieldMeta *OpenDbFieldMeta(const std::string &inDbName);
	//! Open a search record database
	DbSearchRecord *OpenDbSearchRecord(const std::string &inDbName);
	//! Open a document database
	DbDoc *OpenDbDoc(const std::string &inDbName);
	//! Open an index database
	DbNdx *OpenDbNdx(const std::string &inDbName);
	//! Open a map database
	DbMap *OpenDbMap(const std::string &inDbName);
	//! Open a blob database
	DbBlob *OpenDbBlob(const std::string &inDbName);

	// No need for copy and assignment
	DbManager(const DbManager &);
	DbManager &operator=(const DbManager &);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
