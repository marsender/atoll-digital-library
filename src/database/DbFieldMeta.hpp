/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbFieldMeta.hpp

*******************************************************************************/

#ifndef __DbFieldMeta_HPP
#define __DbFieldMeta_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../util/FieldMeta.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Field metadata database
/**
	Logic:
		- Stores metadata infos related to fields
*/
class DbFieldMeta : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbFieldMeta(const DbFieldMeta &);
	DbFieldMeta &operator=(const DbFieldMeta &);

public:
	//! Database constructor.
	DbFieldMeta(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbFieldMeta();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Add a field metadata record
	bool AddFieldMeta(const FieldMeta &inFieldMeta);
	//! Get a field metadata record
	bool GetFieldMeta(FieldMeta &ioFieldMeta);
	//! Del a field metadata record for a given type
	bool DelFieldMeta(const FieldMeta &inFieldMeta);

	//! Get the list of all field metadata records for a given type, or all types if type = eTypFieldNone
	bool GetFieldMetaVector(FieldMetaVector &outVector, eTypField inTypField);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
