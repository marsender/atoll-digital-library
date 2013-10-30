/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbSearchRecord.hpp

*******************************************************************************/

#ifndef __DbSearchRecord_HPP
#define __DbSearchRecord_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../util/SearchRecord.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

//! Search records database
/**
	Logic:
		- Stores search queries and results as records
		- Should be used as a cache to store search results
*/
class DbSearchRecord : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbSearchRecord(const DbSearchRecord &);
	DbSearchRecord &operator=(const DbSearchRecord &);

public:
	//! Database constructor.
	DbSearchRecord(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbSearchRecord();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	//! Add a search record. The record search id will set to the search criteria vector md5 sum
	bool AddSearchRecord(const SearchRecord &inSearchRecord);
	//! Get a search record. The record type and search id must be set
	bool GetSearchRecord(SearchRecord &outSearchRecord);
	//! Del a search record. The record type and search id must be set
	bool DelSearchRecord(const SearchRecord &inSearchRecord);

	//! Get the list of all search records for a given type, or all types if type = eTypSearchRecordNone
	bool GetSearchRecordVector(SearchRecordVector &outVector, eTypSearchRecord inTypSearchRecord);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
