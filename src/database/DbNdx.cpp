/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbNdx.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbNdx.hpp"
#include "DbBlob.hpp"
#include "DbCursor.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
#include <errno.h> // for ENOENT and EEXIST
#include <fstream> // for LoadFwb
#include "unicode/coll.h"
#include "unicode/ustring.h"
using icu::UnicodeString;
using icu::Collator;
//------------------------------------------------------------------------------

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Database %s: %s", getFullDbName().c_str(), x); }
#define DEF_Log2(x, y) { gLog.log(eTypLogError, "Err > Database %s: %s [%s]", getFullDbName().c_str(), x, y); }
#define DEF_DbLog(x) { gLog.log(eTypLogError, "Err > Database %s - DbError(%d) %s - %s", getFullDbName().c_str(), err, GetErrorString(err).c_str(), x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

extern Collator *gCollator;

// Index database
static const std::string cDbNdxFileName("dbndx.db");
static const u_int32_t cDbNdxInMemoryCacheSize = 300 * 1024; // 300k
static const UChar cStrWordPattern[] = { 0x00B7, 'w', 0x00B7, 0 }; // ·w·
//------------------------------------------------------------------------------

extern "C" {

/*
 * Doc: Set the Btree key comparison function
 *   berkeleydb/docs/gsg/C/btree.html#comparators
 *   berkeleydb/docs/api_reference/C/dbset_bt_compare.html
 */
int unicode_bt_compare(Db *db, const Dbt *a, const Dbt *b)
{
	UNUSED(db);

	int32_t l1 = a->get_size() / sizeof(UChar);
	int32_t l2 = b->get_size() / sizeof(UChar);

	const UChar *s1 = (UChar *)a->get_data();
	const UChar *s2 = (UChar *)b->get_data();

	//UTrace("s1", s1, l1);
	//UTrace("s2", s2, l2);

	// Speed up simple char comparisson
	/*{
		int lexiCmp;
		bool isUnicode = false;
		const UChar *p1 = s1, *p2 = s2;
		int32_t len = l1 > l2 ? l2 : l1;
		for (; len--; ++p1, ++p2) {
			if (*p1 > 127 || *p2 > 127) {
				isUnicode = true;
				break;
			}
			lexiCmp = (int)*p1 - (int)*p2;
			if (lexiCmp != 0)
				return lexiCmp;
		}
		if (!isUnicode) {
			lexiCmp = ((long)l1 - (long)l2);
			return lexiCmp;
		}
	}*/

	// Incorrect method, using the UnicodeString compare function from the containers
	// Not valid, as it must return -1, 0 or 1
	/*
	UnicodeString u1((UChar *)s1, l1);
	UnicodeString u2((UChar *)s2, l2);

	fCmpUnicodeString compStruct;
	return compStruct(u1, u2);
	*/

	bool wantDebug = false;
	if (wantDebug) {
		UnicodeString uBuf(s1, l1);
		uBuf += " <=> ";
		uBuf.append(s2, l2);
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogDebug, "Deb > DbNdx Collator compare: %s", buf.c_str());
	}

	if (gCollator == NULL) {
		gLog.log(eTypLogError, "Err > unicode_bt_compare missing collator");
		UnicodeString uBuf(s1, l1);
		int8_t cmp = uBuf.compare(s2, l2);
		return cmp;
	}

	// 0 if string a == string b
	// 1 if string a > string b
	// -1 if string a < string b
	UErrorCode status = U_ZERO_ERROR;
	UCollationResult cmp = gCollator->compare(s1, l1, s2, l2, status);
	if (!U_SUCCESS(status)) {
		UnicodeString uBuf(s1, l1);
		uBuf += " <=> ";
		uBuf.append(s2, l2);
		std::string buf = UnicodeString2String(uBuf);
		gLog.log(eTypLogError, "Err > DbNdx Collator compare error %d: %s", status, buf.c_str());
	}

	if (wantDebug)
		gLog.log(eTypLogDebug, "Deb > DbNdx Collator compare result: %d", cmp);

	return cmp;
}
//------------------------------------------------------------------------------

/*
int lexicographical_bt_compare(Db *db, const Dbt *a, const Dbt *b)
{
	UNUSED(db);

	size_t len = a->get_size() > b->get_size() ? b->get_size() : a->get_size();
	unsigned char *p1 = (unsigned char*)a->get_data();
	unsigned char *p2 = (unsigned char*)b->get_data();
	int cmp;
	for (; len--; ++p1, ++p2) {
		cmp = (int)*p1 - (int)*p2;
		if (cmp != 0)
			return cmp;
	}
	cmp = ((long)a->get_size() - (long)b->get_size());
	return cmp;
}
*/
//------------------------------------------------------------------------------

}; // extern "C"
//------------------------------------------------------------------------------

DbNdx::DbNdx(const DbEnv *inEnv, DbTxn *inTxn,
						 const std::string &inDbName,
						 u_int32_t inPageSize, u_int32_t inFlags,
						 eTypKey inTypKey, bool inClearOnOpen, bool inRemoveOnClose /* = false */)
	: DbWrapper(inEnv, cDbNdxFileName, inDbName, inPageSize, inFlags, inClearOnOpen, inRemoveOnClose),
		mTypKey(inTypKey)
{
	gLog.log(eTypLogDebug, "Deb > DbNdx constructor");

	int err = 0;
	mDbBlob = NULL;

	try {
		switch (mTypKey) {
		case eTypKeyUnicode:
			mKeySize = sizeof(UChar);
			break;
		default:
			mKeySize = 1;
			break;
		}

		// Init the Btree comparison function
		DbWrapper::bt_compare_fn fn = get_bt_compare();
		if (fn)
			db_->set_bt_compare(fn);

		// Set cache sizes low if no DbEnv => in-memory database for tests
		if (inEnv == NULL) {
			db_->set_cachesize(0, cDbNdxInMemoryCacheSize, 1);
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

DbNdx::~DbNdx()
{
	gLog.log(eTypLogDebug, "Deb > DbNdx destructor");

	if (mDbBlob) {
		delete mDbBlob;
	}
}
//------------------------------------------------------------------------------

DbWrapper::bt_compare_fn DbNdx::get_bt_compare() const
{
	switch (mTypKey) {
	case eTypKeyUnicode:
		return unicode_bt_compare;
	default:
		break;
	}

	return NULL;
}
//------------------------------------------------------------------------------

int DbNdx::Open(DbTxn *inTxn, u_int32_t inFlags, int inMode)
{
	int err;

	// Support retrieval from the Btree using record numbers
	DbWrapper::set_flags(DB_RECNUM);

	err = DbWrapper::open(inTxn, DB_BTREE, inFlags, inMode);

	return err;
}
//------------------------------------------------------------------------------

DbBlob *DbNdx::GetDbBlob()
{
	return mDbBlob;
}
//------------------------------------------------------------------------------

void DbNdx::SetDbBlob(DbBlob *inDbBlob)
{
	mDbBlob = inDbBlob;
}
//------------------------------------------------------------------------------

bool DbNdx::AppendCursorData(const EntrySet &inEntrySet, Dbt &ioData, BinaryBuffer &outBuffer, const UnicodeString &inWord)
{
	bool isOk = true;
	size_t size = ioData.get_size();
	char *data = (char *)ioData.get_data();
	unsigned int nbBlob = GetShBuf(data);

	// Cannot work without blob
	if (!mDbBlob || !nbBlob)
		return false;

	if (!EntrySetCanAppendData(inEntrySet, data))
		return false;

	// Init the header with current cursor data
	outBuffer.write(data, size);

	// Init the blob with the last blob from database
	std::string word;
	ConvertUnicodeString2String(word, inWord);
	char *key = new char[word.length() + 20];
	sprintf(key, "%s %d", word.c_str(), nbBlob - 1);
	BinaryBuffer blobBuf;
	isOk = mDbBlob->GetElement(key, blobBuf);
	if (!isOk) {
		//Trace("AppendCursorData key: %s", key);
		DEF_Log2("AppendCursorData: Incorrect blob retrieve from database", key);
		delete [] key;
		return false;
	}

	// Append dec to header and entries to blob
	EntrySetAppendData(outBuffer, blobBuf, inEntrySet);

	// Store the new blob
	size_t pageSize = getPageSize(); // Default page size is 4096 on Ubuntu
	size_t blobSize = DEF_SizeOfEntry * pageSize; // Must be a multiple of the entry size
	size_t blobBufSize = blobBuf.getOccupancy();
	unsigned int nbBlobNew = blobBufSize / blobSize;
	size_t lastSize = blobBufSize % blobSize;
	if (lastSize) {
		nbBlobNew++;
	}
	else {
		lastSize = blobSize;
	}
	const char *bufPos = (const char *)blobBuf.getBuffer();
	size_t bufSize = blobSize;
	BinaryBuffer *bufWrap;
	for (unsigned int i = 0; i < nbBlobNew; i++) {
		if (i == nbBlobNew - 1)
			bufSize = lastSize;
		sprintf(key, "%s %d", word.c_str(), nbBlob + i - 1);
		bufWrap = new BinaryBuffer(bufPos, bufSize, true);
		isOk = mDbBlob->AddElement(key, *bufWrap);
		delete bufWrap;
		bufPos += bufSize;
		if (!isOk) {
			break;
		}
	}
	delete [] key;

	// Store the new number of blob if different
	data = (char *)outBuffer.getBuffer();
	if (nbBlobNew > 1)
		SetShBuf(data, nbBlob + nbBlobNew - 1);

	memset(&ioData, 0, sizeof(ioData));
  ioData.set_data(data);
	ioData.set_size((int32_t)outBuffer.getOccupancy());

	return isOk;
}
//------------------------------------------------------------------------------

bool DbNdx::GetCursorData(void *&outBufferVector, EntrySet &outEntrySet, Dbt &inData, const UnicodeString &inWord)
{
	bool isOk = true;
	BinaryBuffer *binBuf = NULL;
	size_t size = inData.get_size();
	const void *data = inData.get_data();

	// Get the number of blobs
	unsigned int nbBlob = GetShBuf((char *)data);
	//if (mDbBlob && nbBlob == 0) {
	//	return false;
	//}
	if (nbBlob > 0) {
		if (mDbBlob == NULL) {
			DEF_Log("GetCursorData: Missing blob database");
			return false;
		}
		binBuf = new BinaryBuffer();
		binBuf->write(data, size);
		// Get the binary buffer from the blob database
		std::string word;
		ConvertUnicodeString2String(word, inWord);
		char *key = new char[word.length() + 20];
		BinaryBuffer *blobBuf;
		BinaryBufferVector *bufferVector = static_cast<BinaryBufferVector *>(outBufferVector);
		bufferVector = new BinaryBufferVector;
		outBufferVector = bufferVector;
		for (unsigned int i = 0; i < nbBlob; i++) {
			blobBuf = new BinaryBuffer;
			bufferVector->push_back(blobBuf);
			sprintf(key, "%s %d", word.c_str(), i);
			isOk = mDbBlob->GetElement(key, *blobBuf);
			if (!isOk) {
				break;
			}
			binBuf->write(blobBuf->getBuffer(), blobBuf->getOccupancy());
		}
		delete [] key;
		if (isOk) {
			data = binBuf->getBuffer();
			size = binBuf->getOccupancy();
		}
		else {
			DEF_Log("GetCursorData: Incorrect blob retrieve from database");
		}
	}

	if (isOk) {
		size_t nbEnt = BufToEntrySet(outEntrySet, data, size, NULL);
		if (nbEnt == 0) {
			UTrace("DbNdx::GetCursorData", inWord.getBuffer(), inWord.length());
			DEF_Log("GetCursorData: Incorrect buf to entry set conversion");
		}
	}

	if (binBuf)
		delete binBuf;

	return isOk;
}
//------------------------------------------------------------------------------

bool DbNdx::SetCursorData(void *inBufferVector, Dbt &outData, BinaryBuffer &outBuffer, const UnicodeString &inWord, EntrySet &inEntrySet)
{
	// Init ndx size with the header size
	// This is not mandatory but allows bobs to be constants, as they only store entries
	size_t ndxSize;

	EntrySetToBuf(outBuffer, ndxSize, inEntrySet);

	bool isOk = true;
	void *buf = outBuffer.getBuffer();
	size_t size = outBuffer.getOccupancy();
	size_t pageSize = getPageSize(); // Default page size is 4096 on Ubuntu
	size_t blobSize = DEF_SizeOfEntry * pageSize; // Must be a multiple of the entry size
	size_t lastSize;

	// Calculate the number of blobs
	unsigned int nbBlob = 0;
	if (mDbBlob) { // Or we can force a min size: && size > pageSize) {
		nbBlob = (size - ndxSize) / blobSize;
		lastSize = (size - ndxSize) % blobSize;
		if (lastSize) {
			nbBlob++;
		}
		else {
			lastSize = blobSize;
		}
		AppAssertMessage(nbBlob != 0, "Blob number cannot be nul");
	}
	SetShBuf((char *)buf, nbBlob);

	// Store the binary buffer into the blob database
	if (nbBlob > 0) {
		bool storeBlob;
		std::string word;
		ConvertUnicodeString2String(word, inWord);
		char *key = new char[word.length() + 20];
		char *bufPos = static_cast<char*>(buf) + ndxSize;
		size_t bufSize = blobSize;
		BinaryBuffer *bufWrap, *bufIt;
		BinaryBufferVector *bufferVector = static_cast<BinaryBufferVector *>(inBufferVector);
		BinaryBufferVector::const_iterator it, itEnd;
		if (bufferVector) {
			it = bufferVector->begin();
			itEnd = bufferVector->end();
		}
		for (unsigned int i = 0; i < nbBlob; i++) {
			if (i == nbBlob - 1)
				bufSize = lastSize;
			sprintf(key, "%s %d", word.c_str(), i);
			bufWrap = new BinaryBuffer(bufPos, bufSize, true);
			storeBlob = true;
			if (bufferVector && it != itEnd) {
				bufIt = *it;
				if (*bufWrap == *bufIt) {
					storeBlob = false;
					//Trace("Reuse blob %d/%d for: %s", i + 1, nbBlob, word.c_str());
				}
				it++;
			}
			if (storeBlob) {
				isOk = mDbBlob->AddElement(key, *bufWrap);
			}
			delete bufWrap;
			bufPos += bufSize;
			if (!isOk) {
				break;
			}
		}
		delete [] key;
		if (isOk) {
			// Reduce the size of the data to store
			size = ndxSize;
		}
		else {
			// Indicate that there is no blob storage
			SetShBuf((char *)buf, 0);
			DEF_Log("SetCursorData: Incorrect blob storage into database");
		}
		// Voir cursor.del(0); mDbBlob->DelElement(key);
	}

	memset(&outData, 0, sizeof(outData));
  outData.set_data(buf);
	outData.set_size((int32_t)size);

	return isOk;
}
//------------------------------------------------------------------------------

void DbNdx::DeleteBufferVector(void *&inBufferVector)
{
	if (inBufferVector == NULL)
		return;

	BinaryBuffer *bufIt;
	BinaryBufferVector *bufferVector = static_cast<BinaryBufferVector *>(inBufferVector);
	BinaryBufferVector::const_iterator it, itEnd;
	it = bufferVector->begin();
	itEnd = bufferVector->end();
	for (; it != itEnd; ++it) {
		bufIt = *it;
		delete bufIt;
	}
	delete bufferVector;

	inBufferVector = NULL;
}
//------------------------------------------------------------------------------

bool DbNdx::AddWordEntry(const UnicodeString &inWord, const Entry &inEntry)
{
	bool res;
	EntrySet entrySet;

	entrySet.insert(inEntry);
	res = AddWordEntries(inWord, entrySet);

	return res;
}
//------------------------------------------------------------------------------

bool DbNdx::AddWordEntries(const UnicodeString &inWord, const EntrySet &inEntrySet)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	EntrySet entrySet;
	u_int32_t flags;
	BinaryBuffer buf;
	void *bufferVector = NULL;
	bool isAppend = false;
	bool wantTrace = false;

	//gLog.log(eTypLogDebug, "Deb > AddWordEntries %s", getFullDbName().c_str());

	if (inWord.length() == 0) {
		DEF_Log("AddWordEntries: Empty word");
		return false;
	}

	if (inEntrySet.size() == 0) {
		DEF_Log("AddWordEntries: Empty entry set");
		return false;
	}

	if (wantTrace) {
		UTrace("DbNdx::AddWordEntries", inWord.getBuffer(), inWord.length());
	}

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inWord.getBuffer());
	key.set_size(inWord.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists: get it's entries
		// Try the case of apening a new document
		isAppend = AppendCursorData(inEntrySet, data, buf, inWord);
		if (!isAppend) {
			//if (getDbName().find("metadata_") == std::string::npos) {
			//	DEF_DbLog("AddWordEntries: Cannot append data");
			//}
			GetCursorData(bufferVector, entrySet, data, inWord);
		}
		flags = DB_CURRENT;
		break;
	case DB_NOTFOUND:
		flags = DB_KEYFIRST;
		break;
	default:
		DEF_DbLog("AddWordEntries: Get cursor");
		return false;
	}

	if (!isAppend) {
		// Add the new entries in the set
		EntrySet::const_iterator it, itEnd;	// Set iterator
		itEnd = inEntrySet.end();
		for (it = inEntrySet.begin(); it != itEnd; ++it) {
			entrySet.insert(*it);
		}

		// Set new data
		SetCursorData(bufferVector, data, buf, inWord, entrySet);
		DeleteBufferVector(bufferVector);
	}

	// Replace the data part of the record to which the cursor refers
	err = cursor.put(&key, &data, flags);
	if (err != 0) {
		DEF_DbLog("AddWordEntries: Put cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbNdx::GetWordEntries(const UChar *inStr, int32_t inLength, EntrySet &outEntrySet)
{
	UnicodeString word;

	word.setTo(inStr, inLength);
	return GetWordEntries(word, outEntrySet);
}
//------------------------------------------------------------------------------

bool DbNdx::GetWordEntries(const UnicodeString &inWord, EntrySet &outEntrySet)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	void *bufferVector = NULL;
	bool wantTrace = false;

	outEntrySet.clear();

	if (inWord.length() == 0) {
		DEF_Log("GetWordEntries: Empty word");
		return false;
	}

	// Lowercase the word if the database is lowercase
	UnicodeString dbName, word = inWord;
	ConvertString2UnicodeString(dbName, getDbName());
	if (u_strstr(dbName.getTerminatedBuffer(), cStrWordPattern)) {
		word.toLower();
	}

	if (wantTrace) {
		std::string info = "GetWordEntries for ";
		info += getDbName();
		UnicodeString str = word; //inWord;
		UTrace(info.c_str(), str.getTerminatedBuffer());
	}

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)word.getBuffer());
	key.set_size(word.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists: get it's entries
		GetCursorData(bufferVector, outEntrySet, data, word);
		DeleteBufferVector(bufferVector);
		break;
	case DB_NOTFOUND:
		return false;
	default:
		DEF_DbLog("GetWordEntries: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbNdx::DelWordEntries(const UnicodeString &inWord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;

	if (inWord.length() == 0) {
		DEF_Log("DelWordEntries: Empty word");
		return false;
	}

	DbCursor cursor(*this, txn, eTypCursorWrite);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inWord.getBuffer());
	key.set_size(inWord.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists: remove it
		err = cursor.del(0);
		if (err != 0) {
			DEF_DbLog("DelWordEntries: Del cursor");
			return false;
		}
		break;
	case DB_NOTFOUND:
		break;
	default:
		DEF_DbLog("DelWordEntries: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbNdx::DelWordDocEntries(const UnicodeString &inWord, unsigned int inDoc)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	std::string str;
	EntrySet entrySet;
	EntrySet::iterator it, itBak;
	EntrySet::const_iterator itEnd;
	void *bufferVector = NULL;
	size_t size;

	if (inWord.length() == 0) {
		DEF_Log("DelWordDocEntries: Empty word");
		return false;
	}

	DbCursor cursor(*this, txn, eTypCursorWrite);

	// Reset the key
	bool isDoc;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inWord.getBuffer());
	key.set_size(inWord.length() * mKeySize);
	// The get() routine returns the key and data pair, not just the key!
	err = cursor.get(&key, &data, DB_SET);
	switch (err) {
	case 0:
		// The word exists: get it's entries
		GetCursorData(bufferVector, entrySet, data, inWord);
		// Remove entries matching with the doc to remove
		isDoc = false;
		itEnd = entrySet.end();
		for (it = entrySet.begin(); it != itEnd;) {
			if (it->mUiEntDoc == inDoc) {
				itBak = it++;
				entrySet.erase(itBak);
				isDoc = true;
			}
			else
				it++;
		}
		// If the word has no more positions: remove it else store the new word entries
		size = entrySet.size();
		if (size == 0) {
			err = cursor.del(0);
			if (err != 0)
				DEF_DbLog("DelWordDocEntries: Del cursor");
		}
		else if (isDoc) {
			// Set new data
			BinaryBuffer buf;
			SetCursorData(bufferVector, data, buf, inWord, entrySet);

			// Replace the data part of the record to which the cursor refers
			err = cursor.put(&key, &data, DB_CURRENT);
			if (err != 0) {
				DEF_DbLog("DelWordDocEntries: Put cursor");
				return false;
			}
		}
		DeleteBufferVector(bufferVector);
		break;
	case DB_NOTFOUND:
		break;
	default:
		DEF_DbLog("DelWordDocEntries: Get cursor");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbNdx::DelDocEntries(unsigned int inDoc)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	EntrySet entrySet;
	void *bufferVector = NULL;
	size_t size;

	DbCursor cursor(*this, txn, eTypCursorWrite);

	// Initialize the key/data return pair
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	// Walk through the database
	bool isDoc;
	BinaryBuffer buf;
	UnicodeString word;
  EntrySet::iterator it, itBak, itEnd;	// Set iterator
	while ((err = cursor.get(&key, &data, DB_NEXT)) == 0) {
		// Get word
		word.setTo((UChar *)key.get_data(), key.get_size() / mKeySize);
		// Get the word entries
		GetCursorData(bufferVector, entrySet, data, word);
		// Remove entries matching with the doc to remove
		isDoc = false;
		itEnd = entrySet.end();
		for (it = entrySet.begin(); it != itEnd;) {
			if (it->mUiEntDoc == inDoc) {
				itBak = it++;
				entrySet.erase(itBak);
				isDoc = true;
			}
			else
				it++;
		}
		// If the word has no more positions: remove it else store the new word entries
		size = entrySet.size();
		if (size == 0) {
			err = cursor.del(0);
			if (err != 0)
				DEF_DbLog("DelDocEntries: Del cursor");
		}
		else if (isDoc) {
			// Set new data
			SetCursorData(bufferVector, data, buf, word, entrySet);

			// Replace the data part of the record to which the cursor refers
			err = cursor.put(&key, &data, DB_CURRENT);
			if (err != 0) {
				DEF_DbLog("DelDocEntries: Put cursor");
				return false;
			}
		}
		DeleteBufferVector(bufferVector);
	}
	if (err != DB_NOTFOUND) {
		DEF_DbLog("DelDocEntries: Get cursor DB_NEXT");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

/*
	Doc: Retrieving Btree records by logical record number
		dbxml/dbxml/docs/ref/am_conf/bt_recnum.html
	[engine] void FslGetWord(myString &outMsWord, unsigned long index);
*/
bool DbNdx::GetWord(UnicodeString &outWord, unsigned long inRecNum)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
  //Dbc *cursorp;
	db_recno_t recno = inRecNum + 1;
	std::string str;

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Reset the key
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data(&recno);
	key.set_size(sizeof(recno));
	// The get() routine returns the key and data pair, not just the key!
	if ((err = cursor.get(&key, &data, DB_SET_RECNO)) != 0) {
		outWord.remove(); // Clear the word
		std::string msg = "Get cursor DB_SET_RECNO";
		//DEF_DbLog(msg);
		DEF_Exception(AppException::BerkeleyDbError, msg);
	}

	outWord.setTo((UChar *)key.get_data(), key.get_size() / mKeySize);

	return true;
}
//------------------------------------------------------------------------------

unsigned long DbNdx::GetWordRecNum(const UnicodeString &inWord)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	db_recno_t recno;

	DbCursor cursor(*this, txn, eTypCursorRead);

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.set_data((void *)inWord.getBuffer());
	key.set_size(inWord.length() * mKeySize);
	//UTrace("Key", inWord.getTerminatedBuffer());
	// The get() routine returns the key and data pair, not just the key!
	if ((err = cursor.get(&key, &data, DB_SET)) != 0) {
		//DEF_DbLog("GetWordRecNum: Get cursor DB_SET");
		// The key doesn't exist, so we search the nearest
		recno = GetWordRecNumNear(inWord);
		return (unsigned long)recno;
	}
	else {
		// Request the record number, and store it into appropriately
		// sized and aligned local memory.
		memset(&data, 0, sizeof(data));
		data.set_data(&recno);
		data.set_ulen(sizeof(recno));
		data.set_flags(DB_DBT_USERMEM);
		if ((err = cursor.get(&key, &data, DB_GET_RECNO)) != 0) {
			DEF_DbLog("GetWordRecNum: Get cursor DB_GET_RECNO");
		}
	}
	if (recno < 1) {
		DEF_DbLog("GetWordRecNum: Incorrect record number");
	}

	return err == 0 && recno >= 1 ? (unsigned long)(recno - 1) : DEF_LongMax;
}
//------------------------------------------------------------------------------

unsigned long DbNdx::GetWordRecNumNear(const UnicodeString &inWord)
{
  int8_t cmp;
  UnicodeString word;
	int32_t l1, l2;
	const UChar *s1, *s2;
  unsigned long pos, top, bot, count;

	count = Count();
  if (count == 0) {
    return 0;
  }

	// Dichotomy search
  bot = 0;
  top = count - 1;
	s1 = inWord.getBuffer();
	l1 = inWord.length();
  while (bot <= top) {
    pos = (top + bot) / 2;
    GetWord(word, pos);
		// Word comparison
		{
			s2 = word.getBuffer();
			l2 = word.length();
			if (gCollator == NULL) {
				cmp = inWord.compare(s2, l2);
			}
			else {
				// 0 if string a == string b
				// 1 if string a > string b
				// -1 if string a < string b
				UErrorCode status = U_ZERO_ERROR;
				cmp = gCollator->compare(s1, l1, s2, l2, status);
				if (!U_SUCCESS(status)) {
					UnicodeString uBuf = inWord;
					uBuf += " <=> ";
					uBuf.append(s2, l2);
					std::string buf = UnicodeString2String(uBuf);
					gLog.log(eTypLogError, "Err > DbNdx Collator compare near error %d: %s", status, buf.c_str());
				}
			}
		}
    if (cmp > 0)
      bot = pos + 1;
    else if (cmp < 0) {
      if (pos)
        top = pos - 1;
      else {
        top = DEF_LongMax;
        break;
      }
    }
    else
      return pos;
  }
  if (top == DEF_LongMax)
    pos = 0;
  else
    pos = ((top + bot) / 2) + 1;

	return pos;
}
//------------------------------------------------------------------------------

bool DbNdx::GetWordList(UnicodeStringVector &outVector)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;

	outVector.clear();

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Initialize the key/data return pair
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	UnicodeString word;
	// Walk through the database and get the items
	// The get() routine returns the key and data pair, not just the key!
	while ((err = cursor.get(&key, &data, DB_NEXT)) == 0) {
		// Get the key (don't use the data)
		word.setTo((UChar *)key.get_data(), key.get_size() / mKeySize);
		outVector.push_back(word);
		//UTrace("Vector Word", word.getTerminatedBuffer());
	}
	if (err != DB_NOTFOUND) {
		DEF_DbLog("GetWordList: Get cursor DB_NEXT");
		outVector.clear();
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbNdx::GetWordIntList(WordIntVector &outVector, unsigned long inMin, unsigned long inNb)
{
	int err;
	Dbt key, data;
	DbTxn *txn = NULL;
	bool wantTrace = false;

	outVector.clear();

	if (wantTrace) {
		std::string info = getDbName();
		Trace("GetWordIntList for %s", info.c_str());
	}

	DbCursor cursor(*this, txn, eTypCursorRead);

	// Initialize the key/data return pair
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	db_recno_t recno;
	bool isRecNo = false;
	if (inMin > 0) {
		isRecNo = true;
		recno = inMin + 1;
		key.set_data(&recno);
		key.set_size(sizeof(recno));
	}

	WordInt wordInt;
	unsigned long nb = 0;
	// Walk through the database and get the items
	// The get() routine returns the key and data pair, not just the key!
	while (true) {
		if (++nb > inNb) {
			err = DB_NOTFOUND;
			break;
		}
		if (isRecNo) {
			isRecNo = false;
			err = cursor.get(&key, &data, DB_SET_RECNO);
		}
		else
			err = cursor.get(&key, &data, DB_NEXT);
		if (err != 0)
			break;
		// Get the key = word
		wordInt.mWrd.setTo((UChar *)key.get_data(), key.get_size() / mKeySize);
		// Get the number of occurrences from the data = entries
		wordInt.mInt = GetBufEntrySetCount(data.get_data());
		outVector.push_back(wordInt);
		//Trace("%s", wordInt.ToString().c_str());
		//UTrace("Vector Word", wordInt.mWrd.getTerminatedBuffer());
	}
	if (err != DB_NOTFOUND) {
		DEF_DbLog("GetWordIntList: Get cursor DB_NEXT");
		outVector.clear();
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

unsigned long DbNdx::LoadFwb(const std::string &inFileName)
{
	// Clear the database
	DbWrapper::clear();

  std::ifstream fwbFile(inFileName.c_str(), std::ios::in);
  if (!fwbFile) {
		std::string msg = "File " + inFileName + " doesn't exist";
		DEF_Exception(AppException::FileError, msg);
  }

  //char databuf_1[500];
  char databuf_2[500];
	memset(databuf_2, 0, 500);
	memcpy(databuf_2, "bip", 4);

	Dbt key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	const char *sLine;
	UnicodeString uStr;
  std::string stringBuf;
	unsigned long nbLig = 0;
	size_t size = 0;
  while (!fwbFile.eof()) {
		// Get one line
		nbLig++;
    std::getline(fwbFile, stringBuf);

    // Parse the line
    if (!stringBuf.empty()) {
			sLine = stringBuf.c_str() + 1; // Après le 1er car = langue

			// Set key according to the type
			switch (mTypKey) {
			case eTypKeyUnicode:
				uStr = sLine;
				size = uStr.length();
				key.set_data((void *)uStr.getBuffer());
				key.set_size((u_int32_t)size * mKeySize);
				break;
			default:
				size = stringBuf.size() - 1; // Sans le 1er car = langue
				key.set_data((void *)sLine);
				key.set_size((u_int32_t)size);
				break;
			}

      data.set_data(databuf_2);
			data.set_size((u_int32_t)4);

      DbWrapper::put(NULL, &key, &data, 0);
    }
  }

  fwbFile.close();

	return nbLig;
}
//------------------------------------------------------------------------------
