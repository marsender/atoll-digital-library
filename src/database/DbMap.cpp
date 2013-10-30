/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbMap.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbMap.hpp"
#include "DbCursor.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include <errno.h> // for ENOENT and EEXIST
//------------------------------------------------------------------------------

#define DEF_BinaryFormat // Use binary format to read/write integers ?
#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Database %s: %s", getFullDbName().c_str(), x); }
#define DEF_DbLog(x) { gLog.log(eTypLogError, "Err > Database %s - DbError(%d) %s - %s", getFullDbName().c_str(), err, GetErrorString(err).c_str(), x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Map database
static const std::string cDbMapFileName("dbmap.db");
static const u_int32_t cDbMapInMemoryCacheSize = 500 * 1024; // 500k
//------------------------------------------------------------------------------

DbMap::DbMap(const DbEnv *inEnv, DbTxn *inTxn,
						 const std::string &inDbName,
						 u_int32_t inPageSize, u_int32_t inFlags,
						 bool inClearOnOpen, bool inRemoveOnClose /* = false */)
	: DbWrapper(inEnv, cDbMapFileName, inDbName, inPageSize, inFlags,
			inClearOnOpen, inRemoveOnClose)
{
	gLog.log(eTypLogDebug, "Deb > DbMap constructor");

	int err = 0;

	try {
		mKeySize = sizeof(UChar);

		// Set cache sizes low if no DbEnv => in-memory database for tests
		if (inEnv == NULL) {
			db_->set_cachesize(0, cDbMapInMemoryCacheSize, 1);
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

DbMap::~DbMap()
{
	gLog.log(eTypLogDebug, "Deb > DbMap destructor");
}
//------------------------------------------------------------------------------

int DbMap::Open(DbTxn *inTxn, u_int32_t inFlags, int inMode)
{
	int err;

	// Support retrieval from the Btree using record numbers
	//DbWrapper::set_flags(DB_RECNUM);

	err = DbWrapper::open(inTxn, DB_BTREE, inFlags, inMode);

	return err;
}
//------------------------------------------------------------------------------

/*
bool DbMap::AddElement(const UnicodeString &inKey, const BinaryBuffer &inBinaryBuffer)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	u_int32_t flags;

	if (inKey.length() == 0) {
		DEF_Log("AddElement: Empty key");
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
		// The word exists
		flags = DB_CURRENT;
		break;
	case DB_NOTFOUND:
		flags = DB_KEYFIRST;
		break;
	default:
		DEF_DbLog("AddElement: Get cursor");
		return false;
	}

	// Set new data
	memset(&data, 0, sizeof(data));
  data.set_data(inBinaryBuffer.getBuffer());
	data.set_size((int32_t)inBinaryBuffer.getOccupancy());

	// Set the data part of the record to which the cursor refers
	err = cursor.put(&key, &data, flags);
	if (err != 0) {
		DEF_DbLog("AddElement: Put cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbMap::GetElement(const UnicodeString &inKey, BinaryBuffer &outBinaryBuffer)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;

	outBinaryBuffer.reset();

	if (inKey.length() == 0) {
		DEF_Log("GelElement: Empty key");
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
		outBinaryBuffer.set(data.get_data(), data.get_size());
		break;
	case DB_NOTFOUND:
		return false;
	default:
		DEF_DbLog("GetElement: Get cursor");
		return false;
	}

	return true;
}*/
//------------------------------------------------------------------------------

bool DbMap::AddElement(const UnicodeString &inKey, unsigned int inData)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	u_int32_t flags;

	if (inKey.length() == 0) {
		DEF_Log("AddElement: Empty key");
		return false;
	}

#ifdef DEF_BinaryFormat
	char *buf = static_cast<char*>(::malloc(DEF_SizeOfLong));
#else
  char buf[50];
#endif
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
		DEF_DbLog("AddElement: Get cursor");
		return false;
	}

	// Set new data
	memset(&data, 0, sizeof(data));
#ifdef DEF_BinaryFormat
	SetLgBuf(buf, inData);
#else
	sprintf(buf, "%u", inData);
#endif
  data.set_data(buf);
#ifdef DEF_BinaryFormat
		data.set_size(DEF_SizeOfLong);
#else
		data.set_size((int32_t)strlen(buf) + 1); // Must store the null terminated
#endif

	// Set the data part of the record to which the cursor refers
	err = cursor.put(&key, &data, flags);
#ifdef DEF_BinaryFormat
	::free(buf);
#endif
	if (err != 0) {
		DEF_DbLog("AddElement: Put cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbMap::GetElement(const UnicodeString &inKey, unsigned int &outData)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	const char *sData;

	outData = DEF_LongMax;

	if (inKey.length() == 0) {
		DEF_Log("GelElement: Empty key");
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
		sData = static_cast<const char*>(data.get_data());
#ifdef DEF_BinaryFormat
		if (data.get_size() != DEF_SizeOfLong) {
			DEF_Log("Incorrect data storage");
			return false;
		}
		outData = GetLgBuf(sData);
#else
		// Check if the string has been null terminated
		if (!(data.get_size() > 0 && sData[data.get_size() - 1] == 0)) {
			DEF_Log("Incorrect data storage");
			return false;
		}
		// Convert into the right map format
		if (sscanf(sData, "%u", &outData) != 1) {
			DEF_Log("Incorrect data conversion");
			return false;
		}
#endif
		break;
	case DB_NOTFOUND:
		return false;
	default:
		DEF_DbLog("GetElement: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbMap::DelElement(const UnicodeString &inKey)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;

	if (inKey.length() == 0) {
		DEF_Log("DelElement: Empty key");
		return false;
	}

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
			DEF_DbLog("DelElement: Del cursor");
			return false;
		}
		break;
	case DB_NOTFOUND:
		break;
	default:
		DEF_DbLog("DelElement: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbMap::LoadMapFromDb(Common::UnicodeStringToIntMap &outMap)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;

	outMap.clear();

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Initialize the key/data return pair
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	// Walk through the database and add the key/data pairs
	bool isOk = true;
	UnicodeString mapKey;
	const char *sData;
	unsigned int mapData;
	// The get() routine returns the key and data pair, not just the key!
	while ((err = cursor.get(&key, &data, DB_NEXT)) == 0) {
		// Get the key
		mapKey.setTo((UChar *)key.get_data(), key.get_size() / mKeySize);
		// Get the data
		sData = static_cast<const char*>(data.get_data());
#ifdef DEF_BinaryFormat
		if (data.get_size() != DEF_SizeOfLong) {
			DEF_Log("Incorrect data storage");
			isOk = false;
			continue;
		}
		mapData = GetLgBuf(sData);
#else
		// Check if the string has been null terminated
		if (!(data.get_size() > 0 && sData[data.get_size() - 1] == 0)) {
			DEF_Log("Incorrect data storage");
			isOk = false;
			continue;
		}
		// Convert into the right map format
		if (sscanf(sData, "%u", &mapData) != 1) {
			DEF_Log("Incorrect data conversion");
			isOk = false;
			continue;
		}
#endif
		// Add the key => value in the map
		// With insert method: outMap.insert(std::pair<UnicodeString, unsigned int>(mapKey, mapData));
		outMap[mapKey] = mapData;
	}
	if (err != DB_NOTFOUND) {
		DEF_DbLog("Load map: Get cursor DB_NEXT");
		outMap.clear();
		return false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

void DbMap::SaveMapToDb(const Common::UnicodeStringToIntMap &inMap)
{
  // Clear the database content
	clear();

	Dbt key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	size_t size;
#ifdef DEF_BinaryFormat
	char *buf = static_cast<char*>(::malloc(DEF_SizeOfLong));
#else
  char buf[50];
#endif
	const UnicodeString *mapKey;
	unsigned int mapData;
	UnicodeStringToIntMap::const_iterator it = inMap.begin();
	UnicodeStringToIntMap::const_iterator itEnd = inMap.end();
	for (; it != itEnd; ++it) {
		// Set the key
		mapKey = &it->first;
		size = mapKey->length();
		key.set_data((void *)mapKey->getBuffer());
		key.set_size((u_int32_t)size * mKeySize);
		// Set the data
		mapData = it->second;
#ifdef DEF_BinaryFormat
		SetLgBuf(buf, mapData);
#else
		sprintf(buf, "%u", mapData);
#endif
		data.set_data(buf);
#ifdef DEF_BinaryFormat
		data.set_size(DEF_SizeOfLong);
#else
		data.set_size((int32_t)strlen(buf) + 1); // Must store the null terminated
#endif
		// Add the data
    DbWrapper::put(NULL, &key, &data, 0);
	}

#ifdef DEF_BinaryFormat
	::free(buf);
#endif
}
//------------------------------------------------------------------------------
