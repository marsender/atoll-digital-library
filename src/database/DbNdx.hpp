/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbNdx.hpp

*******************************************************************************/

#ifndef __DbNdx_HPP
#define __DbNdx_HPP
//------------------------------------------------------------------------------

#include "../util/Entry.hpp" // First include, for MSVC warnings
#include "DbWrapper.hpp"
//#include "SharedPtr.hpp"
#include "../common/Containers.hpp" // For UnicodeStringVector
#include "../util/WordInt.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

// Common::BinaryBuffer declaration
namespace Common
{
	class BinaryBuffer;
}
//------------------------------------------------------------------------------

namespace Atoll
{
class DbBlob;

//! Index database
/**
	Logic:
		- This database stores words and positions related to a single index
		- Word sorting depends on the comparison key {@link #eTypKey}.
		- Each word has optional entries = positions for this word
*/
class DbNdx : public DbWrapper
{
public:
	//! Btree key comparison type
	enum eTypKey {
		eTypKeyNone,
		eTypKeyDefault, //!< Standard Berkeley DB key
		eTypKeyUnicode, //!< Unicode key, unsing collation comparison
		eTypKeyEnd
	};

private:
	eTypKey mTypKey;
	int32_t mKeySize;
	DbBlob *mDbBlob;

	// No need for copy and assignment
	DbNdx(const DbNdx &);
	DbNdx &operator=(const DbNdx &);

	//! Get the Btree compare function
	DbWrapper::bt_compare_fn get_bt_compare() const;
	//! Get the nearest record number of a word in the database
	unsigned long GetWordRecNumNear(const UnicodeString &inWord);

	//! Append entries into a database cursor
	bool AppendCursorData(const EntrySet &inEntrySet, Dbt &ioData, Common::BinaryBuffer &outBuffer, const UnicodeString &inWord);
	//! Get entries from a database cursor
	bool GetCursorData(void *&outBufferVector, EntrySet &outEntrySet, Dbt &inData, const UnicodeString &inWord);
	//! Set entries into a database cursor
	bool SetCursorData(void *inBufferVector, Dbt &outData, Common::BinaryBuffer &outBuffer, const UnicodeString &inWord, EntrySet &inEntrySet);
	//! Delete the buffer vector
	void DeleteBufferVector(void *&inBufferVector);

public:
	//typedef SharedPtr<DbNdx> Ptr;

	//! Database constructor. DbName is the index name
	DbNdx(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, eTypKey inTypKey, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbNdx();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Get the blob database
	DbBlob *GetDbBlob();
	//! Set a blob database
	void SetDbBlob(DbBlob *inDbBlob);

	bool AddWordEntry(const UnicodeString &inWord, const Entry &inEntry);
	bool AddWordEntries(const UnicodeString &inWord, const EntrySet &inEntrySet);
	bool GetWordEntries(const UnicodeString &inWord, EntrySet &outEntrySet);
	bool GetWordEntries(const UChar *inStr, int32_t inLength, EntrySet &outEntrySet);
	//! Remove one word and all it's entries
	bool DelWordEntries(const UnicodeString &inWord);
	//! Remove one word and all it's entries which are in one doc
	bool DelWordDocEntries(const UnicodeString &inWord, unsigned int inDoc);
	//! Remove all entries in the database, for one doc
	bool DelDocEntries(unsigned int inDoc);

	//! Get a word in the database by it's record number
	bool GetWord(UnicodeString &outWord, unsigned long inRecNum);
	//! Get the record number of a word in the database (first indice is 0)
	unsigned long GetWordRecNum(const UnicodeString &inWord);
	//! Get the list of the words in the database
	bool GetWordList(Common::UnicodeStringVector &outVector);
	//! Get the list of the words with occurrences in the database
	bool GetWordIntList(WordIntVector &outVector, unsigned long inMin, unsigned long inNb);

	//! Load a file of words in the database
	unsigned long LoadFwb(const std::string &inFileName);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
