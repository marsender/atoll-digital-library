/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbDocMeta.hpp

*******************************************************************************/

#ifndef __DbDocMeta_HPP
#define __DbDocMeta_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../util/DocMeta.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Document metadata database
/**
	Logic:
		- Stores metadata infos related to documents
*/
class DbDocMeta : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbDocMeta(const DbDocMeta &);
	DbDocMeta &operator=(const DbDocMeta &);

public:
	//! Database constructor.
	DbDocMeta(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbDocMeta();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Add a document metadata record
	bool AddDocMeta(const DocMeta &inDocMeta);
	//! Get a document metadata record
	bool GetDocMeta(DocMeta &ioDocMeta);
	//! Del a document metadata record
	bool DelDocMeta(unsigned int inDocNum);

	//! Get the list of all document metadata records
	bool GetDocMetaVector(DocMetaVector &outVector);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
