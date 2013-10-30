/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbManager.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbManager.hpp"
#include "DbBlob.hpp"
#include "DbColMeta.hpp"
#include "DbDocMeta.hpp"
#include "DbFieldMeta.hpp"
#include "DbSearchRecord.hpp"
#include "DbDoc.hpp"
#include "DbNdx.hpp"
#include "DbMap.hpp"
#include "../common/StdString.hpp"
#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace std;
using namespace Atoll;
using namespace Common;

// SET_DB_ERRCALL is never defined. This is for debugging only
#ifdef SET_DB_ERRCALL
static void errcall(const DbEnv *dbenv, const char *errpfx, const char *msg)
{
	cout << "message: " << (errpfx ? errpfx : "") << ": " << msg << endl;
}
#endif
//------------------------------------------------------------------------------

DbManager::DbManager(const DbEnv *inDbEnv, const UnicodeString &inColId)
	: mDbEnv(const_cast<DbEnv *>(inDbEnv)),
	mColId(inColId),
	mDbColMetaCurrent(NULL),
	mDbDocMetaCurrent(NULL),
	mDbFieldMetaCurrent(NULL),
	mDbSearchRecordCurrent(NULL),
	mDbDocCurrent(NULL),
	mDbNdxCurrent(NULL),
	mDbMapCurrent(NULL),
	mDbBlobCurrent(NULL),
	mNdxObserverSet(NULL),
	mClearOnOpen(false),
	mRemoveOnClose(false)
{
	gLog.log(eTypLogDebug, "Deb > DbManager constructor");

	// It's assumed that the DbEnv is already open
	if (mDbEnv == NULL) {
		DEF_Exception(AppException::NullPointer, "Null DbEnv pointer in DbManager constructor");
	}

	// Collection name must not be empty
	if (mColId.isEmpty()) {
		DEF_Exception(AppException::InvalidValue, "Empty collection name in DbManager constructor");
	}

	/*
	const char *dbHome = 0;
	mDbEnv->get_home(&dbHome);
	if (dbHome && *dbHome)
		mDbHome = dbHome;
	*/

	u_int32_t dbEnvOpenFlags;
	mDbEnv->get_open_flags(&dbEnvOpenFlags); // &mDbEnvOpenFlags
	/*
	// Allow DB_AUTO_COMMIT for now -- this is handled by
	// using DB_NO_AUTO_COMMIT in CacheDatabase.cpp for temporary DBs.
	// leave code in case there are unforeseen issues...
	// DB_AUTO_COMMIT is in "regular" flags, not open flags
	if (dbEnvOpenFlags & DB_AUTO_COMMIT) {
		DEF_Exception(AppException::InvalidValue,
			"A DbEnv using DB_AUTO_COMMIT cannot be used to construct an XmlManager object.");
	}
	*/
	if (dbEnvOpenFlags & DB_INIT_CDB) {
		u_int32_t envFlags = 0;
		mDbEnv->get_flags(&envFlags);
		if (!(envFlags & DB_CDB_ALLDB)) {
			DEF_Exception(AppException::InvalidValue,
				"A DbEnv opened with DB_INIT_CDB must set the DB_CDB_ALLDB flag when used with Berkeley DB XML.");
		}
	}
#ifdef DB_SET_ERRCALL
	mDbEnv->set_errcall(errcall);
#endif
}
//------------------------------------------------------------------------------

DbManager::~DbManager()
{
	// Close all databases
  CloseAllDb();

	gLog.log(eTypLogDebug, "Deb > DbManager destructor");
}
//------------------------------------------------------------------------------

const UnicodeString &DbManager::GetColId() const
{
	return mColId;
}
//------------------------------------------------------------------------------

std::string DbManager::GetFullDbName(unsigned int inDbName) const
{
	std::string dbName;

	ConvertUnicodeString2String(dbName, mColId);
	dbName += "_";
	StdString::AppendUInt(dbName, inDbName);

	return dbName;
}
//------------------------------------------------------------------------------

std::string DbManager::GetFullDbName(const std::string &inDbName) const
{
	std::string dbName;

	ConvertUnicodeString2String(dbName, mColId);
	dbName += "_";
	dbName += inDbName;

	return dbName;
}
//------------------------------------------------------------------------------

std::string DbManager::GetFullDbName(const UnicodeString &inDbName) const
{
	std::string dbName;
	UnicodeString uDbName;

	uDbName = mColId;
	uDbName += "_";
	uDbName += inDbName;

	ConvertUnicodeString2String(dbName, uDbName);

	return dbName;
}
//------------------------------------------------------------------------------

void DbManager::CloseAllDb()
{
	// Close all databases
	CloseAllBlobDb();
	CloseAllColMetaDb();
	CloseAllDocMetaDb();
	CloseAllFieldMetaDb();
	CloseAllSearchRecordDb();
	CloseAllDocDb();
	CloseAllNdxDb();
	CloseAllMapDb();
}
//------------------------------------------------------------------------------

void DbManager::CloseAllBlobDb()
{
	mDbBlobCurrent = NULL;
  DbBlobMap::const_iterator itMap = mDbBlobMap.begin();
	DbBlobMap::const_iterator itMapEnd = mDbBlobMap.end();
  for (; itMap != itMapEnd; ++itMap) {
		delete itMap->second;
	}
	mDbBlobMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllColMetaDb()
{
	mDbColMetaCurrent = NULL;
  DbColMetaMap::const_iterator itCol = mDbColMetaMap.begin();
	DbColMetaMap::const_iterator itColEnd = mDbColMetaMap.end();
  for (; itCol != itColEnd; ++itCol) {
		delete itCol->second;
	}
	mDbColMetaMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllDocMetaDb()
{
	mDbDocMetaCurrent = NULL;
  DbDocMetaMap::const_iterator itDoc = mDbDocMetaMap.begin();
	DbDocMetaMap::const_iterator itDocEnd = mDbDocMetaMap.end();
  for (; itDoc != itDocEnd; ++itDoc) {
		delete itDoc->second;
	}
	mDbDocMetaMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllFieldMetaDb()
{
	mDbFieldMetaCurrent = NULL;
  DbFieldMetaMap::const_iterator itDoc = mDbFieldMetaMap.begin();
	DbFieldMetaMap::const_iterator itDocEnd = mDbFieldMetaMap.end();
  for (; itDoc != itDocEnd; ++itDoc) {
		delete itDoc->second;
	}
	mDbFieldMetaMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllSearchRecordDb()
{
	mDbSearchRecordCurrent = NULL;
  DbSearchRecordMap::const_iterator itDoc = mDbSearchRecordMap.begin();
	DbSearchRecordMap::const_iterator itDocEnd = mDbSearchRecordMap.end();
  for (; itDoc != itDocEnd; ++itDoc) {
		delete itDoc->second;
	}
	mDbSearchRecordMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllDocDb()
{
	mDbDocCurrent = NULL;
  DbDocMap::const_iterator itDoc = mDbDocMap.begin();
	DbDocMap::const_iterator itDocEnd = mDbDocMap.end();
  for (; itDoc != itDocEnd; ++itDoc) {
		delete itDoc->second;
	}
	mDbDocMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllNdxDb()
{
	mDbNdxCurrent = NULL;
  DbNdxMap::const_iterator itNdx = mDbNdxMap.begin();
	DbNdxMap::const_iterator itNdxEnd = mDbNdxMap.end();
  for (; itNdx != itNdxEnd; ++itNdx) {
		delete itNdx->second;
	}
	mDbNdxMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

void DbManager::CloseAllMapDb()
{
	mDbMapCurrent = NULL;
  DbMapMap::const_iterator itMap = mDbMapMap.begin();
	DbMapMap::const_iterator itMapEnd = mDbMapMap.end();
  for (; itMap != itMapEnd; ++itMap) {
		delete itMap->second;
	}
	mDbMapMap.clear(); // Clear the map
}
//------------------------------------------------------------------------------

DbColMeta *DbManager::GetDbColMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbColMetaCurrent && mDbColMetaCurrent->getDbName() == dbName)
		return mDbColMetaCurrent;

	// Search in the map if the db is already open
	DbColMetaMap::const_iterator it = mDbColMetaMap.find(inDbName);
	if (it != mDbColMetaMap.end()) {
		mDbColMetaCurrent = it->second;
		return mDbColMetaCurrent;
	}

	// Open the db
	mDbColMetaCurrent = OpenDbColMeta(dbName);
	AppAssert(mDbColMetaCurrent->getDbName() == dbName);

	// Add the key => value in the map: db name => db pointer
	mDbColMetaMap[inDbName] = mDbColMetaCurrent;

	return mDbColMetaCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbColMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbColMetaCurrent && mDbColMetaCurrent->getDbName() == dbName)
		mDbColMetaCurrent = NULL;

	// Search in the map if the db is open
	DbColMetaMap::iterator it = mDbColMetaMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbColMetaMap.end()) {
		delete it->second;
		mDbColMetaMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbColMeta(const UnicodeString &inDbName)
{
	DbColMeta *dbColMeta = GetDbColMeta(inDbName);
	dbColMeta->remove();
	CloseDbColMeta(inDbName);
}
//------------------------------------------------------------------------------

DbDocMeta *DbManager::GetDbDocMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbDocMetaCurrent && mDbDocMetaCurrent->getDbName() == dbName)
		return mDbDocMetaCurrent;

	// Search in the map if the db is already open
	DbDocMetaMap::const_iterator it = mDbDocMetaMap.find(inDbName);
	if (it != mDbDocMetaMap.end()) {
		mDbDocMetaCurrent = it->second;
		return mDbDocMetaCurrent;
	}

	// Open the db
	mDbDocMetaCurrent = OpenDbDocMeta(dbName);
	AppAssert(mDbDocMetaCurrent->getDbName() == dbName);

	// Add the key => value in the map: db name => db pointer
	mDbDocMetaMap[inDbName] = mDbDocMetaCurrent;

	return mDbDocMetaCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbDocMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbDocMetaCurrent && mDbDocMetaCurrent->getDbName() == dbName)
		mDbDocMetaCurrent = NULL;

	// Search in the map if the db is open
	DbDocMetaMap::iterator it = mDbDocMetaMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbDocMetaMap.end()) {
		delete it->second;
		mDbDocMetaMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbDocMeta(const UnicodeString &inDbName)
{
	DbDocMeta *dbDocMeta = GetDbDocMeta(inDbName);
	dbDocMeta->remove();
	CloseDbDocMeta(inDbName);
}
//------------------------------------------------------------------------------

DbFieldMeta *DbManager::GetDbFieldMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbFieldMetaCurrent && mDbFieldMetaCurrent->getDbName() == dbName)
		return mDbFieldMetaCurrent;

	// Search in the map if the db is already open
	DbFieldMetaMap::const_iterator it = mDbFieldMetaMap.find(inDbName);
	if (it != mDbFieldMetaMap.end()) {
		mDbFieldMetaCurrent = it->second;
		return mDbFieldMetaCurrent;
	}

	// Open the db
	mDbFieldMetaCurrent = OpenDbFieldMeta(dbName);
	AppAssert(mDbFieldMetaCurrent->getDbName() == dbName);

	// Add the key => value in the map: db name => db pointer
	mDbFieldMetaMap[inDbName] = mDbFieldMetaCurrent;

	return mDbFieldMetaCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbFieldMeta(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbFieldMetaCurrent && mDbFieldMetaCurrent->getDbName() == dbName)
		mDbFieldMetaCurrent = NULL;

	// Search in the map if the db is open
	DbFieldMetaMap::iterator it = mDbFieldMetaMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbFieldMetaMap.end()) {
		delete it->second;
		mDbFieldMetaMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbFieldMeta(const UnicodeString &inDbName)
{
	DbFieldMeta *dbFieldMeta = GetDbFieldMeta(inDbName);
	dbFieldMeta->remove();
	CloseDbFieldMeta(inDbName);
}
//------------------------------------------------------------------------------

DbSearchRecord *DbManager::GetDbSearchRecord(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbSearchRecordCurrent && mDbSearchRecordCurrent->getDbName() == dbName)
		return mDbSearchRecordCurrent;

	// Search in the map if the db is already open
	DbSearchRecordMap::const_iterator it = mDbSearchRecordMap.find(inDbName);
	if (it != mDbSearchRecordMap.end()) {
		mDbSearchRecordCurrent = it->second;
		return mDbSearchRecordCurrent;
	}

	// Open the db
	mDbSearchRecordCurrent = OpenDbSearchRecord(dbName);
	AppAssert(mDbSearchRecordCurrent->getDbName() == dbName);

	// Add the key => value in the map: db name => db pointer
	mDbSearchRecordMap[inDbName] = mDbSearchRecordCurrent;

	return mDbSearchRecordCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbSearchRecord(const UnicodeString &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbSearchRecordCurrent && mDbSearchRecordCurrent->getDbName() == dbName)
		mDbSearchRecordCurrent = NULL;

	// Search in the map if the db is open
	DbSearchRecordMap::iterator it = mDbSearchRecordMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbSearchRecordMap.end()) {
		delete it->second;
		mDbSearchRecordMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbSearchRecord(const UnicodeString &inDbName)
{
	DbSearchRecord *dbSearchRecord = GetDbSearchRecord(inDbName);
	dbSearchRecord->remove();
	CloseDbSearchRecord(inDbName);
}
//------------------------------------------------------------------------------

DbDoc *DbManager::GetDbDoc(unsigned int inDocNum)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDocNum));

	// Check if the current db is the right one
	if (mDbDocCurrent && mDbDocCurrent->getDbName() == dbName)
		return mDbDocCurrent;

	// Search in the map if the db is already open
	DbDocMap::const_iterator it = mDbDocMap.find(inDocNum);
	if (it != mDbDocMap.end()) {
		mDbDocCurrent = it->second;
		return mDbDocCurrent;
	}

	// Open the db
	mDbDocCurrent = OpenDbDoc(dbName);
	AppAssert(mDbDocCurrent->getDbName() == dbName);

	// Add the key => value in the map: db name => db pointer
	mDbDocMap[inDocNum] = mDbDocCurrent;

	return mDbDocCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbDoc(unsigned int inDocNum)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDocNum));

	// Check if the current db is the right one
	if (mDbDocCurrent && mDbDocCurrent->getDbName() == dbName)
		mDbDocCurrent = NULL;

	// Search in the map if the db is open
	DbDocMap::iterator it = mDbDocMap.find(inDocNum); // Cannot be const_iterator
	if (it != mDbDocMap.end()) {
		delete it->second;
		mDbDocMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbDoc(unsigned int inDocNum)
{
	DbDoc *dbDoc = GetDbDoc(inDocNum);
	dbDoc->remove();
	CloseDbDoc(inDocNum);
}
//------------------------------------------------------------------------------

DbNdx *DbManager::GetDbNdx(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbNdxCurrent && mDbNdxCurrent->getDbName() == dbName)
		return mDbNdxCurrent;

	// Search in the map if the db is already open
	DbNdxMap::const_iterator it = mDbNdxMap.find(inDbName);
	if (it != mDbNdxMap.end()) {
		mDbNdxCurrent = it->second;
		return mDbNdxCurrent;
	}

	// Open the db
	mDbNdxCurrent = OpenDbNdx(dbName);

	// Add the key => value in the map: db name => db pointer
	mDbNdxMap[inDbName] = mDbNdxCurrent;

	// Add the db name in the observer
	if (mNdxObserverSet) {
		StdStringSet::const_iterator itFind = mNdxObserverSet->find(inDbName);
		if (itFind == mNdxObserverSet->end()) {
			mNdxObserverSet->insert(inDbName);
		}
	}

	return mDbNdxCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbNdx(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbNdxCurrent && mDbNdxCurrent->getDbName() == dbName)
		mDbNdxCurrent = NULL;

	// Search in the map if the db is open
	DbNdxMap::iterator it = mDbNdxMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbNdxMap.end()) {
		delete it->second;
		mDbNdxMap.erase(it);
	}
}
//------------------------------------------------------------------------------

void DbManager::RemoveDbNdx(const std::string &inDbName)
{
	DbNdx *dbNdx = GetDbNdx(inDbName);
	dbNdx->remove();
	CloseDbNdx(inDbName);
}
//------------------------------------------------------------------------------

void DbManager::SetNdxObserver(Common::StdStringSet *inStringSet)
{
	mNdxObserverSet = inStringSet;

	// Nothing else to do for a clear
	if (mNdxObserverSet == NULL)
		return;

	// Clear the index db map
	CloseAllNdxDb();
}
//------------------------------------------------------------------------------

DbMap *DbManager::GetDbMap(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbMapCurrent && mDbMapCurrent->getDbName() == dbName)
		return mDbMapCurrent;

	// Search in the map if the db is already open
	DbMapMap::const_iterator it = mDbMapMap.find(inDbName);
	if (it != mDbMapMap.end()) {
		mDbMapCurrent = it->second;
		return mDbMapCurrent;
	}

	// Open the db
	mDbMapCurrent = OpenDbMap(dbName);

	// Add the key => value in the map: db name => db pointer
	mDbMapMap[inDbName] = mDbMapCurrent;

	return mDbMapCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbMap(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbMapCurrent && mDbMapCurrent->getDbName() == dbName)
		mDbMapCurrent = NULL;

	// Search in the map if the db is open
	DbMapMap::iterator it = mDbMapMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbMapMap.end()) {
		delete it->second;
		mDbMapMap.erase(it);
	}
}
//------------------------------------------------------------------------------

DbBlob *DbManager::GetDbBlob(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbBlobCurrent && mDbBlobCurrent->getDbName() == dbName)
		return mDbBlobCurrent;

	// Search in the map if the db is already open
	DbBlobMap::const_iterator it = mDbBlobMap.find(inDbName);
	if (it != mDbBlobMap.end()) {
		mDbBlobCurrent = it->second;
		return mDbBlobCurrent;
	}

	// Open the db
	mDbBlobCurrent = OpenDbBlob(dbName);

	// Add the key => value in the map: db name => db pointer
	mDbBlobMap[inDbName] = mDbBlobCurrent;

	return mDbBlobCurrent;
}
//------------------------------------------------------------------------------

void DbManager::CloseDbBlob(const std::string &inDbName)
{
	// Init the full db name
	std::string dbName(GetFullDbName(inDbName));

	// Check if the current db is the right one
	if (mDbBlobCurrent && mDbBlobCurrent->getDbName() == dbName)
		mDbBlobCurrent = NULL;

	// Search in the map if the db is open
	DbBlobMap::iterator it = mDbBlobMap.find(inDbName); // Cannot be const_iterator
	if (it != mDbBlobMap.end()) {
		delete it->second;
		mDbBlobMap.erase(it);
	}
}
//------------------------------------------------------------------------------

DbColMeta *DbManager::OpenDbColMeta(const std::string &inDbName)
{
	DbColMeta *dbColMeta = new DbColMeta(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbColMeta->Count();

	return dbColMeta;
}
//------------------------------------------------------------------------------

DbDocMeta *DbManager::OpenDbDocMeta(const std::string &inDbName)
{
	DbDocMeta *dbDocMeta = new DbDocMeta(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbDocMeta->Count();

	return dbDocMeta;
}
//------------------------------------------------------------------------------

DbFieldMeta *DbManager::OpenDbFieldMeta(const std::string &inDbName)
{
	DbFieldMeta *dbFieldMeta = new DbFieldMeta(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbFieldMeta->Count();

	return dbFieldMeta;
}
//------------------------------------------------------------------------------

DbSearchRecord *DbManager::OpenDbSearchRecord(const std::string &inDbName)
{
	DbSearchRecord *dbSearchRecord = new DbSearchRecord(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbSearchRecord->Count();

	return dbSearchRecord;
}
//------------------------------------------------------------------------------

DbDoc *DbManager::OpenDbDoc(const std::string &inDbName)
{
	DbDoc *dbDoc = new DbDoc(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbDoc->Count();

	return dbDoc;
}
//------------------------------------------------------------------------------

DbNdx *DbManager::OpenDbNdx(const std::string &inDbName)
{
	DbNdx *dbNdx = new DbNdx(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		DbNdx::eTypKeyUnicode, // Database word comparison type
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbNdx->Count();

	// Associate a blob database, except for metadata (for time saving only)
	if (inDbName.find("metadata_") == std::string::npos) {
		DbBlob *dbBlob = OpenDbBlob(inDbName);
		dbNdx->SetDbBlob(dbBlob);
	}

	return dbNdx;
}
//------------------------------------------------------------------------------

DbMap *DbManager::OpenDbMap(const std::string &inDbName)
{
	DbMap *dbMap = new DbMap(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbMap->Count();

	return dbMap;
}
//------------------------------------------------------------------------------

DbBlob *DbManager::OpenDbBlob(const std::string &inDbName)
{
	DbBlob *dbBlob = new DbBlob(
		mDbEnv, // environment or NULL for in-memory database
		NULL, // No transaction
		inDbName, // Database name
		0, // Default page size
		0, // Flags
		mClearOnOpen, // Clear database on open
		mRemoveOnClose); // Remove the database on close

	//unsigned long count = dbBlob->Count();

	return dbBlob;
}
//------------------------------------------------------------------------------

/*
#include <errno.h>
// Callback function for using pr_* functions from C.
int __db_pr_callback(void *handle, const void *str_arg)
{
	char *str;
	FILE *f;

	str = (char *)str_arg;
	f = (FILE *)handle;

	if (strcmp(str, "\\00") == 0)
		return 0;

	if (fprintf(f, "%s", str) != (int)strlen(str))
		return (EIO);

	return (0);
}*/
//------------------------------------------------------------------------------

/*
#include "DbCursor.hpp"
bool DbManager::ShowSubs()
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	bool isOk = true;

	DB_ENV *dbenv = mDbEnv->get_DB_ENV();
	DBT *dbt = key.get_DBT();
	//return false;

	//char *dopt = "a";
	//DB *dbp = db->getDb().get_DB();
	//err = __db_dumptree(dbp, NULL, dopt, NULL);

	// Show sub
	DbWrapper *db = new DbWrapper(mDbEnv, "", "atoll_ndx_", "Y ~ 2 0", 0, 0);
	err = db->open(
		txn, // No transaction
		DB_UNKNOWN, // Db type
		DB_RDONLY, // Flags
		0); // Mode
	if (err != 0) {
		delete db;
		return false;
	}

	DbCursor *cursor = new DbCursor(*db, txn, eTypCursorRead);
	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	// The get() routine returns the key and data pair, not just the key!
	err = cursor->get(&key, &data, DB_SET);
	while ((err = cursor->get(&key, &data, DB_IGNORE_LEASE | DB_NEXT)) == 0) {
		if ((err = dbenv->prdbt(dbt, 1, NULL, stdout, __db_pr_callback, 0)) != 0) {
			isOk = false;
			break;
			//dbp->errx(dbp, NULL);
			//return (1);
		}
	}
	if (err != DB_NOTFOUND) {
		isOk = false;
		//DEF_DbLog("DelDocEntries: Get cursor DB_NEXT");
		//return false;
	}
	delete cursor;

	delete db;

	return isOk;
}*/
//------------------------------------------------------------------------------

/*  (doesn't work)
void DbManager::RemoveDb(const std::string &inDbName)
{
	int err = 0;

	try {
		err = mDbEnv->dbremove(NULL, inDbName.c_str(), 0, 0);
	}
	catch(DbException &e) {
		if (e.get_errno() == ENOENT)
			err = ENOENT;
		else
			throw AppException(e);
	}
	if (err) {
		if (err == ENOENT) {
			std::string msg = "RemoveDb: database not found: " + inDbName;
			DEF_Exception(AppException::BerkeleyDbError, msg);
		}
		else
			throw AppException(err);
	}
	else {
		gLog.log(eTypLogDebug, "Deb > Container %s removed", inDbName.c_str());
	}
}*/
//------------------------------------------------------------------------------
