/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbWrapper.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbWrapper.hpp"
#include <sstream> // For std::ostringstream
#ifdef DEF_DbWrapperLoad
#include "db_rdbt.h"
#endif
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > DbWrapper: %s", x); }

// Provide consistent handling of deadlock and DB errors
#define DEF_HandleDbErrors(x) \
	catch (DbException &e) { \
		err = e.get_errno(); \
		std::ostringstream oss; \
		if (err == DB_LOCK_DEADLOCK) { \
			oss << "DB lock dead lock during " << (x) << ": " << getFullDbName() << "\n" << e.what(); \
			DEF_Exception(AppException::BerkeleyDbError, oss.str()); \
		} \
		gLog.log(eTypLogError, "Err > Unknown exception during %s '%s': %s", (x), getFullDbName().c_str(), e.what()); \
	} \
	catch (std::exception &e) { \
		gLog.log(eTypLogError, "Err > Unknown exception during %s '%s': %s", (x), getFullDbName().c_str(), e.what()); \
	}

//#define DEF_DefaultPageSize (8 * 1024)
//------------------------------------------------------------------------------

static bool checkCDB(DbEnv *env)
{
	if (env) {
		u_int32_t envFlags;
		env->get_flags(&envFlags);
		if (envFlags & DB_CDB_ALLDB)
			return true;
	}
	return false;
}
//------------------------------------------------------------------------------

DbWrapper::DbWrapper(const DbEnv *inEnv, const std::string &inFileName,
	const std::string &inDbName, u_int32_t inPageSize, u_int32_t inFlags,
	bool inClearOnOpen, bool inRemoveOnClose)
{
	flags_ = 0;
	fileName_ = inFileName;
	dbName_ = inDbName;
	fullDbName_ = inFileName + "_" + inDbName;
	pageSize_ = inPageSize; // Default page size is 4096 on Ubuntu
	db_ = NULL;
	environment_ = (DbEnv *)inEnv;
	mClearOnOpen = inClearOnOpen; // Clear database on open
	mRemoveOnClose = inRemoveOnClose; // Remove the database on close

	mFileName = fileName_.length() == 0 ? NULL : fileName_.c_str();
	mDbName = dbName_.length() == 0 ? NULL : dbName_.c_str();

	db_ = new Db(environment_, inFlags);
	if (!db_)
		DEF_Exception(AppException::NoMemoryError, "Unable to allocate memory for database object");
	if (environment_)
		setFlags(environment_);
	else
		DEF_Log("Database constructor called with a NULL environment");
}
//------------------------------------------------------------------------------

// Destructor
DbWrapper::~DbWrapper()
{
	try {
		if (mRemoveOnClose)
			remove();
		else
			close(0);
	}
	catch (DbException &e) {
		gLog.log(eTypLogError, "Err > Db exception during database close: %s", e.what());
	}
	catch (std::exception &e) {
		// Exceptions thrown from destructors are a bad thing.
		gLog.log(eTypLogError, "Err > Unknown exception during database close: %s", e.what());
	}
	delete db_;
}
//------------------------------------------------------------------------------

std::string DbWrapper::GetErrorString(int inNumErr)
{
	if (environment_ == NULL)
		return "";

	std::string str(environment_->strerror(inNumErr));

	return str;
}
//------------------------------------------------------------------------------

void DbWrapper::setFlags(DbEnv *env)
{
	AppAssert(env);

	u_int32_t oflags = 0;
	env->get_open_flags(&oflags);
	if (checkCDB(environment_))
		setTransacted();
	if (oflags & DB_INIT_CDB)
		setCDBEnv();
	if (oflags & DB_INIT_LOCK)
		setLocking();
}
//------------------------------------------------------------------------------

int DbWrapper::open(DbTxn *inTxn, DBTYPE inType, u_int32_t inFlags, int inMode)
{
	int err = 0;

	if (pageSize_ > 0)
		db_->set_pagesize(pageSize_);

	// Voir la liste des flags dans le fichier dbxml
	//   C:\Work\Prog\xml\dbxml\dbxml\include\dbxml\DbXmlFwd.hpp

	//if (inFlags & ATOLL_CHKSUM) {
	//	db_->set_flags(DB_CHKSUM);
	//	inFlags &= ~ATOLL_CHKSUM;
	//}

	//if (inFlags & ATOLL_ENCRYPT) {
	//	db_->set_flags(DB_ENCRYPT);
	//	inFlags &= ~ATOLL_ENCRYPT;
	//}

	if (inFlags & DB_TXN_NOT_DURABLE) {
		db_->set_flags(DB_TXN_NOT_DURABLE);
		inFlags &= ~DB_TXN_NOT_DURABLE;
	}

	if (inTxn)
		setTransacted();
	try {
		gLog.log(eTypLogDebug, "Deb > Database open: %s", getFullDbName().c_str());
		// The file is opened in the directory specified by the environment
		err = db_->open(inTxn, mFileName, mDbName, inType, inFlags, inMode);
	}
	DEF_HandleDbErrors("open");

	if (err == 0) {
		setNeedsClose();
		if (pageSize_ == 0) {
			// Find out the page size of the underlying database.
			// We don't use stat because it will fail with
			// 'Invalid argument' if the open is within a txn.
			// Default page size is 4096 on Ubuntu
			pageSize_ = db_->get_DB()->pgsize;
		}
		// Clear database on opening
		if (mClearOnOpen)
			clear();
	}

	return err;
}
//------------------------------------------------------------------------------

// Private member used to close a database.
// Called from the class destructor
int DbWrapper::close(u_int32_t inFlags)
{
	if (!needsClose())
		return 0;

	// db.close kills the db handle even if it fails
	clearNeedsClose();

  // Close the db
	int err;
  try {
    err = db_->close(inFlags);
		gLog.log(eTypLogDebug, "Deb > Database close: %s", getFullDbName().c_str());
  }
	catch(DbException &e) {
		err = e.get_errno();
		gLog.log(eTypLogError, "Err > DB exception during Database close: %s - DbError(%d) %s - %s",
			getFullDbName().c_str(), err, GetErrorString(err).c_str(), e.what());
	}

	return err;
}
//------------------------------------------------------------------------------

unsigned long DbWrapper::Count()
{
	int err;
	DbTxn *txn = NULL;
	DB_BTREE_STAT *statp;

	if ((err = stat(txn, &statp, 0)) != 0) {
		DEF_Log("Count: DB stat");
	}

	u_int32_t count = (unsigned long)statp->bt_ndata;
	::free(statp);

	return count;
}
//------------------------------------------------------------------------------

int DbWrapper::set_flags(u_int32_t inFlags)
{
	int ret;

	if ((ret = db_->set_flags(inFlags)) != 0) {
		if (environment_)
			environment_->err(ret, "set_flags error: %lu", inFlags);
		else
			DEF_Log("set_flags error");
	}

	return ret;
}
//------------------------------------------------------------------------------

int DbWrapper::clear()
{
	int err;
	u_int32_t count;
	DbTxn *txn = NULL;

	// After the call, count contains the number of deleted records
	err = DbWrapper::truncate(txn, &count);

	return err;
}
//------------------------------------------------------------------------------

#ifdef DEF_DbWrapperDump
extern "C"
{
	/*
	 * pr_callback - C++ callback function for using pr_* functions from C++.
	 */
	static int pr_callback(void *handle, const void *str_arg) {
		std::ostream &out = *(std::ostream *)handle;
		const char *str = (const char *)str_arg;

		out << str;
		return (0);
	}
} /* extern "C" */
//------------------------------------------------------------------------------
#endif

#ifdef DEF_DbWrapperDump
int DbWrapper::dump(std::ostream *out)
{
	int ret, err = 0;

	if ((ret = open(NULL, DB_UNKNOWN, DB_RDONLY, 0)) == 0) {
		DB *dbp = db_->get_DB();
		err = dbp->dump(dbp, NULL, pr_callback, out, 0, 1);
		ret = close(0);
	}
	if (ret == 0)
		ret = err;

	return (ret);
}
//------------------------------------------------------------------------------
#endif

#ifdef DEF_DbWrapperLoad
extern "C"
{
	/*
	 * Implementation of READ_FN for reading from a C++ istream.
	 * Reads at most 'len' characters into 'buf' up to the first 'until' characater
	 * (if non-zero). The terminator (if found) is discarded, and the string is nul
	 * terminated if 'len' > 1.
	 * Returns: zero on success, DB_NOTFOUND if 'until' != 0 and not found, or
	 * EOF on EOF
	 */
	static int
	read_callback(char *buf, size_t len, char until, void *handle) {
		std::istream &in = *(std::istream *)handle;
		char *p = buf;
		char c;
		size_t bytes_read;

		for (bytes_read = 0; bytes_read < len; ++bytes_read) {
			if (!in.get(c) || (until != 0 && c == until))
				break;
			else
				*p++ = c;
		}

		if (bytes_read < len)
			*p = '\0';

		if (!in && bytes_read == 0)
			return (EOF);
		else if (until != 0 && c != until)
			return (DB_NOTFOUND);
		else
			return (0);
	}
} /* extern "C" */
//------------------------------------------------------------------------------
#endif

#ifdef DEF_DbWrapperLoad
int DbWrapper::load(std::istream *in, unsigned long *lineno)
{
	int version, ret, t_ret;
	DBTYPE dbtype;
	char *subdb;
	u_int32_t read_flags, tflags;
	Dbt key, data;
	db_recno_t recno, datarecno;
	DB_ENV *dbenv = environment_ ? environment_->get_DB_ENV() : 0;

	if ((ret = __db_rheader(dbenv, db_->get_DB(), &dbtype,
				&subdb, &version, &read_flags, read_callback, in, lineno)) != 0)
		goto err;

	/* We always print with keys */
	if (!(read_flags & DB_READ_HASKEYS)) {
		db_->errx("Invalid DbXml dump: keys missing");
		ret = EINVAL;
		goto err;
	}

	if ((ret = open(NULL, dbtype, DB_CREATE|DB_EXCL, 0)) != 0)
		goto err;

	/* Initialize the key/data pair. */
	if (dbtype == DB_RECNO || dbtype == DB_QUEUE) {
		key.set_size(sizeof(recno));
		key.set_data(&datarecno);
	}
	else {
		key.set_ulen(1024);
		key.set_data((void *)malloc(key.get_ulen()));
	}
	data.set_ulen(1024);
	data.set_data((void *)malloc(data.get_ulen()));
	if (key.get_data() == NULL || data.get_data() == NULL) {
		db_->err(ENOMEM, NULL);
		goto err;
	}
	
	// work around gcc optimizer issue that seems to modify
	// read_flags (4.1.1 on 64-bit linux)
	tflags = read_flags;
	/* Get each key/data pair and add them to the database. */
	for (recno = 1;; ++recno) {
		if ((ret = __db_rdbt(dbenv, key.get_DBT(), data.get_DBT(),
				     tflags, read_callback, in, lineno)) != 0) {
			if (ret == EOF)
				ret = 0;
			break;
		}

		switch (ret = db_->put(NULL, &key, &data, 0)) {
		case 0:
			break;
		case DB_KEYEXIST:
			db_->errx("line %d: key already exists, not loaded:", *lineno);
			dbenv->prdbt(key.get_DBT(), tflags & DB_READ_PRINTABLE,
				     0, &std::cerr, pr_callback, 0);
			break;
		default:
			db_->err(ret, NULL);
			goto err;
		}
	}

 err:	/* Close the database. */
	if ((t_ret = close(0)) != 0 && ret == 0)
		ret = t_ret;

	/* Free allocated memory. */
	if (subdb != NULL)
		free(subdb);
	if (dbtype != DB_RECNO && dbtype != DB_QUEUE)
		free(key.get_data());
	if (data.get_data() != NULL)
		free(data.get_data());

	return (ret);
}
//------------------------------------------------------------------------------
#endif

int DbWrapper::verify(std::ostream *outStream, u_int32_t inFlags)
{
	int err = 0;

	clearNeedsClose(); // db.verify kills the db handle even if it fails.

	try {
		err = db_->verify(mFileName, mDbName, outStream, inFlags);
	}
	DEF_HandleDbErrors("verify");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::sync()
{
	int err = 0;

	try {
		err = db_->sync(0);
	}
	DEF_HandleDbErrors("sync");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::cursor(DbTxn *inTxn, Dbc **outCursor, u_int32_t inFlags)
{
	int err = 0;

	try {
		err = db_->cursor(inTxn, outCursor, inFlags);
	}
	DEF_HandleDbErrors("cursor");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::put(DbTxn *inTxn, Dbt *inKey, Dbt *inData, u_int32_t inFlags)
{
	int err = 0;

	try {
		err = db_->put(inTxn, inKey, inData, inFlags);
	}
	DEF_HandleDbErrors("put");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::get(DbTxn *inTxn, Dbt *inKey, Dbt *inData, u_int32_t inFlags) const
{
	int err = 0;

	try {
		err = const_cast<Db*>(db_)->get(inTxn, inKey, inData, inFlags);
	}
	catch(DbMemoryException &e) {
		err = DB_BUFFER_SMALL;
		std::ostringstream oss;
		oss << "DB exception " << "get" << ": " << getFullDbName() << "\n";
		oss << e.what();
		DEF_Log(oss.str().c_str());
	}
	DEF_HandleDbErrors("get");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::del(DbTxn *inTxn, Dbt *inKey, u_int32_t inFlags)
{
	int err = 0;

	try {
		err = db_->del(inTxn, inKey, inFlags);
	}
	DEF_HandleDbErrors("del");

	return err;
}
//------------------------------------------------------------------------------

// Empties the database, discarding all records it contains.
// The number of records discarded from the database is returned in countp.
int DbWrapper::truncate(DbTxn *inTxn, u_int32_t *inCountp)
{
	int err = 0;

	try {
		err = db_->truncate(inTxn, inCountp, 0 /* flags */);
	}
	DEF_HandleDbErrors("truncate");

	return err;
}
//------------------------------------------------------------------------------

// Creates a statistical structure and copies a pointer to it
// into user-specified memory locations
int DbWrapper::stat(DbTxn *inTxn, void *inStatStruct, u_int32_t inFlags)
{
	int err = 0;

	try {
		err = db_->stat(inTxn, inStatStruct, inFlags);
	}
	DEF_HandleDbErrors("stat");

	return err;
}
//------------------------------------------------------------------------------

// Removes the database
int DbWrapper::remove()
{
  close(0);

	int err = 0;
	try {
		// Create a new db handle, as close has killed the db handle
	  Db db(environment_, 0);
		err = db.remove(mFileName, mDbName, 0 /* flags */);
    // remove kills the db handle
	}
	DEF_HandleDbErrors("remove");

	return err;
}
//------------------------------------------------------------------------------

int DbWrapper::key_range(DbTxn *inTxn, Dbt *inKey, DB_KEY_RANGE *inKeyRange,
		    u_int32_t inFlags) const
{
	int err = 0;

	try {
		err = const_cast<Db*>(db_)->key_range(inTxn, inKey, inKeyRange, inFlags);
	}
	DEF_HandleDbErrors("key_range");

	return err;
}
//------------------------------------------------------------------------------
