/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbUtil.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "DbUtil.hpp"
#include "DbWrapper.hpp"
#include <db_cxx.h>
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

#define DEF_NbCacheSizeMB 512 // Cache size, in Mega Bytes
//------------------------------------------------------------------------------

// Return if the database contains subdatabases.
/*
int is_sub(DB *dbp, int *yesno)
{
	DB_BTREE_STAT *btsp;
	DB_HASH_STAT *hsp;
	int ret;

	switch (dbp->type) {
	case DB_BTREE:
	case DB_RECNO:
		if ((ret = dbp->stat(dbp, NULL, &btsp, DB_FAST_STAT)) != 0) {
			dbp->err(dbp, ret, "DB->stat");
			return (ret);
		}
		*yesno = btsp->bt_metaflags & DB_AM_SUBDB ? 1 : 0; // BTM_SUBDB
		::free(btsp);
		break;
	case DB_HASH:
		if ((ret = dbp->stat(dbp, NULL, &hsp, DB_FAST_STAT)) != 0) {
			dbp->err(dbp, ret, "DB->stat");
			return (ret);
		}
		*yesno = hsp->hash_metaflags & DB_AM_SUBDB ? 1 : 0; // DB_HASH_SUBDB
		::free(hsp);
		break;
	case DB_QUEUE:
		break;
	case DB_UNKNOWN:
	default:
		dbp->errx(dbp, "unknown database type");
		return (1);
	}
	return (0);
}*/
//------------------------------------------------------------------------------

DbEnv *DbUtil::StaticCreateEnvironment(const char *inPrefix, const char *inHome, const char *inDataDir)
{
	int ret;
	DbEnv *dbenv;

	try {
		// Create an environment and initialize it for additional error reporting.
		dbenv = new DbEnv(0);

		// Error
		dbenv->set_errfile(stderr);
		//dbEnv->set_error_stream(&cerr);
		if (inPrefix)
			dbenv->set_errpfx(inPrefix);

		// Specify the shared memory buffer pool cachesize
		// The size of the cache is set to gbytes gigabytes plus bytes
		// The ncache parameter is the number of caches to create
		u_int32_t gbytes = 0;
		u_int32_t bytes = DEF_NbCacheSizeMB * 1024 * 1024;
		int ncache = 1;
		dbenv->set_cachesize(gbytes, bytes, ncache);

		// Boost some default resource sizes
		// (locks only work at environment creation time)
		dbenv->set_lk_max_lockers(4096);
		dbenv->set_lk_max_locks(20000);
		dbenv->set_lk_max_objects(2048);

		// Databases are in a subdirectory.
		if (inDataDir)
			dbenv->set_data_dir(inDataDir);

		// Env open flags
		//   dbxml/dbxml/docs/gsg_xml/cxx/dbandcontainer.html#environment
		/*
			DB_CREATE     |  // Create the environment if it does not exist
			DB_RECOVER    |  // Run normal recovery.
			DB_INIT_LOCK  |  // Initialize the locking subsystem
			DB_INIT_LOG   |  // Initialize the logging subsystem
			DB_INIT_TXN   |  // Initialize the transactional subsystem. This also turns on logging.
			DB_INIT_MPOOL |  // Initialize the memory pool (in-memory cache)
			DB_THREAD;       // Cause the environment to be free-threaded
		*/
		bool wantTxn = false; // Utilisation des transactions ?
		u_int32_t envFlags = DB_CREATE | DB_INIT_MPOOL | DB_THREAD;
		if (wantTxn) {
			envFlags |= DB_INIT_LOCK // Initialize the locking subsystem
								| DB_INIT_LOG // Initialize the logging subsystem
								| DB_INIT_TXN; // Initialize transactions
								// DB_TXN_NOSYNC | DB_TXN_WRITE_NOSYNC
								// DB_RECOVER ?
		}

		// Open the environment
		if ((ret = dbenv->open(inHome, envFlags, 0)) != 0) {
			dbenv->close(0);
			delete dbenv;
			dbenv = NULL;
			std::string msg = "Environment open error: ";
			msg += inHome;
			DEF_Exception(AppException::BerkeleyDbError, msg);
		}
	}
	catch (AppException &e) {
		gLog.log(eTypLogError, "Err > Db static create environment exception: %s", e.what());
		return NULL;
	}

	return dbenv;
}
//------------------------------------------------------------------------------

void DbUtil::StaticDeleteEnvironment(DbEnv *&inDbEnv)
{
	if (inDbEnv == NULL)
		return;

	inDbEnv->close(0);
	delete inDbEnv;
	inDbEnv = NULL;
}
//------------------------------------------------------------------------------

bool DbUtil::StaticGetDbList(DbEnv &inDbEnv, DbList &outDbList)
{
	DB_ENV *dbenv;
	DB_MPOOL_FSTAT **fsp, **tfsp;
	DB_MPOOL_STAT *gsp;
	int ret;
	u_int32_t flags = DB_STAT_CLEAR;

	// Clear the output list
	outDbList.clear();

	dbenv = inDbEnv.get_DB_ENV();
	ret = dbenv->memp_stat(dbenv, &gsp, &fsp, flags);
	if (ret != 0)
		return false;

	std::string name;
	for (tfsp = fsp; fsp != NULL && *tfsp != NULL; ++tfsp) {
		name = (*tfsp)->file_name;
		outDbList.push_back(name);
		//gLog.log(eTypLogDebug, "Deb > Pool File: %s", name.c_str());
		/*
		__db_dl(dbenv, "Page size", (u_long)(*tfsp)->st_pagesize);
		__db_dl(dbenv,
		    "Requested pages mapped into the process' address space",
		    (u_long)(*tfsp)->st_map);
		__db_dl_pct(dbenv, "Requested pages found in the cache",
		    (u_long)(*tfsp)->st_cache_hit, DB_PCT((*tfsp)->st_cache_hit,
		    (*tfsp)->st_cache_hit + (*tfsp)->st_cache_miss), NULL);
		__db_dl(dbenv, "Requested pages not found in the cache",
		    (u_long)(*tfsp)->st_cache_miss);
		__db_dl(dbenv, "Pages created in the cache",
		    (u_long)(*tfsp)->st_page_create);
		__db_dl(dbenv, "Pages read into the cache",
		    (u_long)(*tfsp)->st_page_in);
		__db_dl(dbenv,
		    "Pages written from the cache to the backing file",
		    (u_long)(*tfsp)->st_page_out);
		*/
	}

	// __os_ufree
	if (dbenv->db_free) {
		dbenv->db_free(fsp);
		dbenv->db_free(gsp);
	}
	else {
		::free(fsp);
		::free(gsp);
	}

	return true;
}
//------------------------------------------------------------------------------

bool DbUtil::StaticRemoveDbList(DbEnv &inDbEnv, DbList &inDbList)
{
	int err;
	bool isOk = true;
	std::string dbName;

  DbList::const_iterator it, itEnd;	// List iterator
	itEnd = inDbList.end();
  for (it = inDbList.begin(); it != itEnd; ++it) {
		dbName = *it;
		DbWrapper *db = new DbWrapper(&inDbEnv, "", dbName.c_str(), 0, 0, false, false);
		err = db->open(
			NULL, // No transaction
			DB_UNKNOWN, // Db type
			DB_RDONLY, // Flags
			0); // Mode
		if (err != 0)
			isOk = false;
		else
			db->remove();
		delete db;
	}

	return isOk;
}
//------------------------------------------------------------------------------
