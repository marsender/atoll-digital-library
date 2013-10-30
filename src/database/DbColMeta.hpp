/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbColMeta.hpp

*******************************************************************************/

#ifndef __DbColMeta_HPP
#define __DbColMeta_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../util/ColMeta.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Collection metadata database
/**
	Logic:
		- Stores metadata infos related to collections
*/
class DbColMeta : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbColMeta(const DbColMeta &);
	DbColMeta &operator=(const DbColMeta &);

public:
	//! Database constructor.
	DbColMeta(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbColMeta();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Add a collection metadata record
	bool AddColMeta(const ColMeta &inColMeta);
	//! Get a collection metadata record
	bool GetColMeta(ColMeta &ioColMeta);
	//! Del a collection metadata record
	bool DelColMeta(const UnicodeString &inKey);

	//! Get the list of all collection metadata records
	bool GetColMetaVector(ColMetaVector &outVector);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
