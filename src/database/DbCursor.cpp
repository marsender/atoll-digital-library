/*******************************************************************************

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

DbCursor.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbCursor.hpp"
#include <sstream> // For std::ostringstream 
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > DbCursor: %s", x); }

// Provide consistent handling of deadlock and DB errors
#define DEF_HandleDbErrors(x) \
	catch (DbException &e) { \
		err = e.get_errno(); \
		std::ostringstream oss; \
		if (err == DB_LOCK_DEADLOCK) { \
			oss << "DB lock dead lock during " << (x) << ": " << db_->getFullDbName() << "\n" << e.what(); \
			DEF_Exception(AppException::BerkeleyDbError, oss.str()); \
		} \
		gLog.log(eTypLogError, "Err > DB exception during %s '%s': %s", (x), db_->getFullDbName().c_str(), e.what()); \
	} \
	catch (std::exception &e) { \
		gLog.log(eTypLogError, "Err > Unknown exception during %s '%s': %s", (x), db_->getFullDbName().c_str(), e.what()); \
	}

//#define DB_CURSOR_CREATE_MASK(flags) ((flags) & ~(DB_RMW))
//#define DB_CURSOR_GET_MASK(flags) ((flags) & ~(DB_READ_COMMITTED))
//------------------------------------------------------------------------------

DbCursor::DbCursor(DbWrapper &db, DbTxn *txn, TypCursor type, u_int32_t flags)
	: dbc_(0),
	db_(&db)
{
	int err;

	if (type == eTypCursorWrite && db_->getEnvironment() != 0) {
		// If we're in a CDS environment then the cursor used to
		// delete the keys must be created with the DB_WRITECURSOR flag.
		if (db.isCDBEnv())
			flags |= DB_WRITECURSOR;
	}

	try {
		err = db.cursor(txn, &dbc_, (flags) & ~(DB_RMW)); // DB_CURSOR_CREATE_MASK(flags)
	}
	DEF_HandleDbErrors("DbCursor constructor");
}
//------------------------------------------------------------------------------

DbCursor::~DbCursor()
{
	if (dbc_)
		dbc_->close();
}
//------------------------------------------------------------------------------

void DbCursor::close()
{
	if (dbc_) {
		dbc_->close();
		dbc_ = 0;
	}
}
//------------------------------------------------------------------------------

int DbCursor::get(Dbt *key, Dbt *data, u_int32_t flags)
{
	int err = 0;

	try {
		err = dbc_->get(key, data, flags);
	}
	DEF_HandleDbErrors("DbCursor get");
	/*
	catch (DbMemoryException &) {
		err = DB_BUFFER_SMALL;
	}
	catch (DbException &de) {
		err = de.get_errno();
	}
	*/

	return err;
}
//------------------------------------------------------------------------------

int DbCursor::put(Dbt *key, Dbt *data, u_int32_t flags)
{
	int err = 0;

	try {
		err = dbc_->put(key, data, flags);
	}
	DEF_HandleDbErrors("DbCursor put");

	return err;
}
//------------------------------------------------------------------------------

int DbCursor::del(u_int32_t flags)
{
	int err = 0;

	try {
		err = dbc_->del(flags);
	}
	DEF_HandleDbErrors("DbCursor del");

	return err;
}
//------------------------------------------------------------------------------
