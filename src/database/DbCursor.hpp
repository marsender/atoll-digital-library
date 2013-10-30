/*******************************************************************************

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

DbCursor.hpp

*******************************************************************************/

#ifndef __DbCursor_HPP
#define	__DbCursor_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{

typedef enum eTypCursor {
	eTypCursorRead,
	eTypCursorWrite
} TypCursor;

//! Provides exception safety for a Berkeley Db cursor
class DbCursor
{
public:
	DbCursor(DbWrapper &db, DbTxn *txn, TypCursor type, u_int32_t flags = 0);
	~DbCursor();
	void close();
	int get(Dbt *key, Dbt *data, u_int32_t flags);
	int put(Dbt *key, Dbt *data, u_int32_t flags);
	int del(u_int32_t flags);

private:
	// no need for copy and assignment
	DbCursor(const DbCursor&);
	DbCursor &operator=(const DbCursor&);

	Dbc *dbc_;
	DbWrapper *db_;
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
