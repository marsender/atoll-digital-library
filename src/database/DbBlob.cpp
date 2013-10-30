/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbBlob.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbBlob.hpp"
#include "DbCursor.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/Zlib.hpp"
#include <errno.h> // for ENOENT and EEXIST
//------------------------------------------------------------------------------

#define DEF_BinaryFormat // Use binary format to read/write integers ?
#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Database %s: %s", getFullDbName().c_str(), x); }
#define DEF_DbLog(x) { gLog.log(eTypLogError, "Err > Database %s - DbError(%d) %s - %s", getFullDbName().c_str(), err, GetErrorString(err).c_str(), x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

// Want zlib record compression / decompression ?
const bool cWantZlib = true;
// Blob database
static const std::string cDbBlobFileName("dbblob.db");
static const u_int32_t cDbBlobInMemoryCacheSize = 500 * 1024; // 500k
//------------------------------------------------------------------------------

DbBlob::DbBlob(const DbEnv *inEnv, DbTxn *inTxn,
						 const std::string &inDbName,
						 u_int32_t inPageSize, u_int32_t inFlags,
						 bool inClearOnOpen, bool inRemoveOnClose /* = false */)
	: DbWrapper(inEnv, cDbBlobFileName, inDbName, inPageSize, inFlags,
			inClearOnOpen, inRemoveOnClose)
{
	gLog.log(eTypLogDebug, "Deb > DbBlob constructor");

	int err = 0;

	try {
		mKeySize = 1;

		// Set cache sizes low if no DbEnv => in-memory database for tests
		if (inEnv == NULL) {
			db_->set_cachesize(0, cDbBlobInMemoryCacheSize, 1);
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

DbBlob::~DbBlob()
{
	gLog.log(eTypLogDebug, "Deb > DbBlob destructor");
}
//------------------------------------------------------------------------------

int DbBlob::Open(DbTxn *inTxn, u_int32_t inFlags, int inMode)
{
	int err;

	// Support retrieval from the Btree using record numbers
	//DbWrapper::set_flags(DB_RECNUM);

	err = DbWrapper::open(inTxn, DB_BTREE, inFlags, inMode);

	return err;
}
//------------------------------------------------------------------------------

bool DbBlob::AddElement(const std::string &inKey, const BinaryBuffer &inBinaryBuffer)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	u_int32_t flags;
	size_t bufSize;

	if (inKey.length() == 0) {
		DEF_Log("AddElement: Empty key");
		return false;
	}

	bufSize = inBinaryBuffer.getOccupancy();
	if (bufSize == 0) {
		DEF_Log("AddElement: Empty data");
		return false;
	}

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inKey.c_str());
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

	void *bufData;
	BinaryBuffer *binaryBuffer = NULL;
	if (cWantZlib) {
		char buf[DEF_SizeOfLong];
		binaryBuffer = new BinaryBuffer;
		unsigned long length = (unsigned long)inBinaryBuffer.getOccupancy();
		SetLgBuf(buf, length);
		binaryBuffer->write(buf, DEF_SizeOfLong);
		ZlibCompress(*binaryBuffer, (const char *)inBinaryBuffer.getBuffer(), length);
		bufData = binaryBuffer->getBuffer();
		bufSize = binaryBuffer->getOccupancy();
	}
	else {
		bufData = inBinaryBuffer.getBuffer();
	}

	// Set new data
	memset(&data, 0, sizeof(data));
  data.set_data(bufData);
	data.set_size((int32_t)bufSize);

	// Set the data part of the record to which the cursor refers
	err = cursor.put(&key, &data, flags);
	if (binaryBuffer) {
		delete binaryBuffer;
	}
	if (err != 0) {
		DEF_DbLog("AddElement: Put cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbBlob::GetElement(const std::string &inKey, BinaryBuffer &outBinaryBuffer)
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
	key.set_data((void *)inKey.c_str());
	key.set_size(inKey.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The key exists: get it's data
		{
			// Zlib decompression
			const char *bufData = (char *)data.get_data();
			size_t bufSize = data.get_size();
			char *uncompr = NULL;
			if (cWantZlib) {
				unsigned long length = GetLgBuf(bufData);
				bufData += DEF_SizeOfLong;
				if (!ZlibDeflate(uncompr, length, (void *)bufData, bufSize)) {
					::free(uncompr);
					return 0;
				}
				bufData = uncompr;
				bufSize = length;
			}
			outBinaryBuffer.write(bufData, bufSize);
			if (uncompr)
				::free(uncompr);
		}
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

bool DbBlob::DelElement(const std::string &inKey)
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
	key.set_data((void *)inKey.c_str());
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
