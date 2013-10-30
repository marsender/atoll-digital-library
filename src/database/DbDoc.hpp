/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbDoc.hpp

*******************************************************************************/

#ifndef __DbDoc_HPP
#define __DbDoc_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../util/DocRecord.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Document database
/**
	Logic:
		- This database stores the document pages in records
*/
class DbDoc : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbDoc(const DbDoc &);
	DbDoc &operator=(const DbDoc &);

public:
	//! Database constructor. DbName is the document number
	DbDoc(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbDoc();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Add a document record
	bool AddDocRecord(const UnicodeString &inKey, const DocRecord &inDocRecord);
	//! Get a document record
	bool GetDocRecord(const UnicodeString &inKey, DocRecord &outDocRecord);
	//! Del a document record
	bool DelDocRecord(const UnicodeString &inKey);

	//! Get the list of all document records
	bool GetDocRecordVector(DocRecordVector &outVector);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
