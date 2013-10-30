/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbSearchRecord.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbSearchRecord.hpp"
#include "DbCursor.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include <errno.h> // for ENOENT and EEXIST
//------------------------------------------------------------------------------

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Database %s: %s", getFullDbName().c_str(), x); }
#define DEF_DbLog(x) { gLog.log(eTypLogError, "Err > Database %s - DbError(%d) %s - %s", getFullDbName().c_str(), err, GetErrorString(err).c_str(), x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Search record database
static const std::string cDbSearchRecordFileName("dbsearchrecord.db");
static const u_int32_t cDbSearchRecordInMemoryCacheSize = 500 * 1024; // 500k
//------------------------------------------------------------------------------

DbSearchRecord::DbSearchRecord(const DbEnv *inEnv, DbTxn *inTxn,
						 const std::string &inDbName,
						 u_int32_t inPageSize, u_int32_t inFlags,
						 bool inClearOnOpen, bool inRemoveOnClose /* = false */)
	: DbWrapper(inEnv, cDbSearchRecordFileName, inDbName, inPageSize, inFlags,
			inClearOnOpen, inRemoveOnClose)
{
	gLog.log(eTypLogDebug, "Deb > DbSearchRecord constructor");

	int err = 0;

	try {
		mKeySize = sizeof(UChar);

		// Set cache sizes low if no DbEnv => in-memory database for tests
		if (inEnv == NULL) {
			db_->set_cachesize(0, cDbSearchRecordInMemoryCacheSize, 1);
		}

		// Allow database creation ?
		u_int32_t flags = DB_CREATE; //&= ~DB_XA_CREATE; DB_RDONLY

		// Open the Db object
		err = this->Open(inTxn, flags, /* default mode */ 0);
	}
	catch (DbException &e) {
		err = e.get_errno();
		// Fall through
	}
	catch (AppException &) {
		if (inTxn)
			inTxn->abort();
		throw;
	}
	if (err != 0) {
		if (inTxn)
			inTxn->abort();
		std::string msg = getFileName();
		if (err == EEXIST) {
			msg += ": container already exists";
			DEF_Exception(AppException::BerkeleyDbError, msg);
		}
		else if (err == ENOENT) {
			msg += ": container not found, or not a container";
			DEF_Exception(AppException::BerkeleyDbError, msg);
		}
		msg += ": container open error";
		DEF_Exception(AppException::BerkeleyDbError, msg);
	}
}
//------------------------------------------------------------------------------

DbSearchRecord::~DbSearchRecord()
{
	gLog.log(eTypLogDebug, "Deb > DbSearchRecord destructor");
}
//------------------------------------------------------------------------------

int DbSearchRecord::Open(DbTxn *inTxn, u_int32_t inFlags, int inMode)
{
	int err;

	// Support retrieval from the Btree using record numbers
	//DbWrapper::set_flags(DB_RECNUM);

	err = DbWrapper::open(inTxn, DB_BTREE, inFlags, inMode);

	return err;
}
//------------------------------------------------------------------------------

bool DbSearchRecord::AddSearchRecord(const SearchRecord &inSearchRecord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	u_int32_t flags;

	// Serialize the record in a binary buffer
	// Must be done first, as it updates the search id with the search criteria vector md5 sum
	BinaryBuffer binaryBuffer;
	inSearchRecord.ToBinaryBuffer(binaryBuffer);

	// Now check if the search record is valid
	if (!inSearchRecord.IsValid()) {
		return false;
	}

	// The key comes from the input record
	UnicodeString inKey;
	inSearchRecord.GetSearchRecordKey(inKey);
	//UTrace("AddSearchRecord Key", inKey.getBuffer(), inKey.length());

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inKey.getBuffer());
	key.set_size(inKey.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists
		flags = DB_CURRENT;
		break;
	case DB_NOTFOUND:
		flags = DB_KEYFIRST;
		break;
	default:
		DEF_DbLog("AddRecord: Get cursor");
		return false;
	}

	// Set new data
	memset(&data, 0, sizeof(data));
  data.set_data(binaryBuffer.getBuffer());
	data.set_size((int32_t)binaryBuffer.getOccupancy());

	// Set the data part of the record to which the cursor refers
	err = cursor.put(&key, &data, flags);
	if (err != 0) {
		DEF_DbLog("AddRecord: Put cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbSearchRecord::GetSearchRecord(SearchRecord &ioSearchRecord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;

	// The key comes from the input record
	UnicodeString inKey;
	ioSearchRecord.GetSearchRecordKey(inKey);
	//UTrace("GetSearchRecord Key", inKey.getBuffer(), inKey.length());

	ioSearchRecord.Clear();

	if (inKey.length() == 0) {
		DEF_Log("GetRecord: Empty key");
		return false;
	}

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inKey.getBuffer());
	key.set_size(inKey.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The key exists: get it's data
		ioSearchRecord.FromBinaryCharBuffer(static_cast<const char*>(data.get_data()), data.get_size());
		// Check the record validity
		/*if (ioSearchRecord.mSearchRecordId != inKey) {
			//DEF_Log("GetRecord: Incorrect search record");
			std::string searchRecordId = UnicodeString2String(ioSearchRecord.mSearchRecordId);
			std::string key = UnicodeString2String(inKey);
			gLog.log(eTypLogError, "Err > Database %s: %s (key='%s' != SearchRecordId='%s')", getFullDbName().c_str(),
				"GetRecord: Incorrect search record", key.c_str(), searchRecordId.c_str());
			return false;
		}*/
		break;
	case DB_NOTFOUND:
		return false;
	default:
		DEF_DbLog("GetRecord: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbSearchRecord::DelSearchRecord(const SearchRecord &inSearchRecord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;

	if (!inSearchRecord.IsValid()) {
		return false;
	}

	// The key comes from the input record
	UnicodeString inKey;
	inSearchRecord.GetSearchRecordKey(inKey);

	DbCursor cursor(*this, txn, eTypCursorWrite);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inKey.getBuffer());
	key.set_size(inKey.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists: remove it
		err = cursor.del(0);
		if (err != 0) {
			DEF_DbLog("DelRecord: Del cursor");
			return false;
		}
		break;
	case DB_NOTFOUND:
		break;
	default:
		DEF_DbLog("DelRecord: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbSearchRecord::GetSearchRecordVector(SearchRecordVector &outVector, eTypSearchRecord inTypSearchRecord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;

	outVector.clear();

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Initialize the key/data return pair
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	// Walk through the database and add the data
	bool isOk = true;
	SearchRecord searchRecord;
	// The get() routine returns the key and data pair, not just the key!
	while ((err = cursor.get(&key, &data, DB_NEXT)) == 0) {
		// Get the data (don't use the key)
		searchRecord.FromBinaryCharBuffer(static_cast<const char*>(data.get_data()), data.get_size());
		// Check the type
		if (inTypSearchRecord != searchRecord.mTypSearchRecord && inTypSearchRecord != eTypSearchRecordNone)
			continue;
		// Add the data in the vector
		outVector.push_back(searchRecord);
	}
	if (err != DB_NOTFOUND) {
		DEF_DbLog("Get search record list: Get cursor DB_NEXT");
		outVector.clear();
		return false;
	}

	return isOk;
}
//------------------------------------------------------------------------------
