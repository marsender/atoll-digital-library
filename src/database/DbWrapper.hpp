/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbWrapper.hpp

*******************************************************************************/

#ifndef __DbWrapper_HPP
#define __DbWrapper_HPP

#include <string>
#include <db_cxx.h>
//------------------------------------------------------------------------------

namespace Atoll
{

//! Berkeley DB database wrapper
class DbWrapper
{
public:
	typedef int(*bt_compare_fn)(Db *, const Dbt *, const Dbt *);

	DbWrapper(const DbEnv *inEnv, const std::string &inFileName,
					 const std::string &inDbName,
					 u_int32_t inPageSize, u_int32_t inFlags,
					 bool inClearOnOpen, bool inRemoveOnClose);
  virtual ~DbWrapper();

	//! Open the database
	int open(DbTxn *inTxn, DBTYPE inType, u_int32_t inFlags, int inMode);
  //! Close the database
	int close(u_int32_t inFlags);

	//! Get the number of records in the database
	unsigned long Count();

	//! Set database flags
	int set_flags(u_int32_t inFlags);
  //! Clear the database content
	int clear();
	//! Remove the database
	int remove();
	//int dump(std::ostream *out);
	//int load(std::istream *in, unsigned long *lineno);
	int verify(std::ostream *outStream, u_int32_t inFlags);
	int sync();
	int cursor(DbTxn *inTxn, Dbc **outCursor, u_int32_t inFlags);
	int put(DbTxn *inTxn, Dbt *inKey, Dbt *inData, u_int32_t inFlags);
	int get(DbTxn *inTxn, Dbt *inKey, Dbt *inData, u_int32_t inFlags) const;
	int del(DbTxn *inTxn, Dbt *inKey, u_int32_t inFlags);
	int stat(DbTxn *inTxn, void *inStatStruct, u_int32_t inFlags);
	int key_range(DbTxn *inTxn, Dbt *inKey, DB_KEY_RANGE *inKeyRange, u_int32_t inFlags) const;

	// Getters
	//const Db &getDb() { return *db_; }
	//const Db *getDbPtr() { return db_; }
	u_int32_t getPageSize() const { return pageSize_; }
	const DbEnv *getEnvironment() const { return environment_; }
	//! Get the database file name
	const std::string &getFileName() const { return fileName_; }
	//! Get the database name
	const std::string &getDbName() const { return dbName_; }
	//! Get the full database name (file name + database name)
	const std::string &getFullDbName() const { return fullDbName_; }

	bool isTransacted() const { 
		return ((flags_ & DBW_TRANSACTED) != 0);
	}
	void setTransacted() { flags_ |= DBW_TRANSACTED; }
	
	bool isCDBEnv() const { 
		return ((flags_ & DBW_CDB_ENV) != 0);
	}
	void setCDBEnv() { flags_ |= DBW_CDB_ENV; }
	
	bool isLocking() const { 
		return ((flags_ & DBW_LOCKING) != 0);
	}
	void setLocking() { flags_ |= DBW_LOCKING; }
	
	bool needsClose() const { 
		return ((flags_ & DBW_NEEDS_CLOSE) != 0);
	}
	void setNeedsClose() { flags_ |= DBW_NEEDS_CLOSE; }
	void clearNeedsClose() { flags_ &= ~DBW_NEEDS_CLOSE; }

	//! Return the database error string corresponding to an error number
	std::string GetErrorString(int inNumErr);

protected:
	u_int32_t flags_;
	u_int32_t pageSize_;
	Db *db_;
	DbEnv *environment_;

private:
	bool mClearOnOpen, mRemoveOnClose;
	std::string fileName_;
	std::string dbName_;
	std::string fullDbName_;
	const char *mFileName, *mDbName;

	// Make sure the default constructor is private. We don't want it used.
  DbWrapper() {}
	// No need for copy and assignment
	DbWrapper(const DbWrapper&);
	DbWrapper &operator=(const DbWrapper &);
	enum flags {
		DBW_NEEDS_CLOSE = 1,
		DBW_TRANSACTED = 2,
		DBW_LOCKING = 4,
		DBW_CDB_ENV = 8
	};
	void setFlags(DbEnv *env);
	int truncate(DbTxn *inTxn, u_int32_t *inCountp);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
