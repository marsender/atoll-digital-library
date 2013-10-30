/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineEnv.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "EngineApi.hpp"
#include "EngineEnv.hpp"
#include "../database/DbUtil.hpp"
#include <db_cxx.h>
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const EngineEnv &inEngineEnv)
{
	inStream << inEngineEnv.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

EngineEnv::EngineEnv()
{
	mDbEnv = NULL;
}
//------------------------------------------------------------------------------

EngineEnv::~EngineEnv()
{
	if (mDbEnv) {
		DbUtil::StaticDeleteEnvironment(mDbEnv);
	}
}
//------------------------------------------------------------------------------

bool EngineEnv::IsValid() const
{
	bool isOk = true;

	if (!mDbEnv) {
		gLog.log(eTypLogError, "Err > EngineEnv: Null db env");
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool EngineEnv::CreateDbEnv(const std::string &inDbHome)
{
	// Delete the previous environment
	if (mDbEnv) {
		DbUtil::StaticDeleteEnvironment(mDbEnv);
	}

	if (!FileSystem::DirectoryExists(inDbHome)) {
		gLog.log(eTypLogError, "Err > Incorrect db home: %s", inDbHome.c_str());
		return false;
	}

	// Create the dababase environment
	// May run in an infinite loop if the database is corrupted
	const char *databasePrefix = GetLibName();
	mDbEnv = DbUtil::StaticCreateEnvironment(databasePrefix, inDbHome.c_str(), NULL);

	return true;
}
//------------------------------------------------------------------------------

const DbEnv *EngineEnv::GetDbEnv() const
{
	return mDbEnv;
}
//------------------------------------------------------------------------------

const char *EngineEnv::GetDbHome()
{
	const char *dbHome = NULL;

	if (mDbEnv)
		mDbEnv->get_home(&dbHome);

	return dbHome;
}
//------------------------------------------------------------------------------

const UnicodeString &EngineEnv::GetColId() const
{
	return mColId;
}
//------------------------------------------------------------------------------

void EngineEnv::SetColId(const UnicodeString &inColId)
{
	mColId = inColId;
}
//------------------------------------------------------------------------------

const UnicodeString &EngineEnv::GetUser() const
{
	return mUser;
}
//------------------------------------------------------------------------------

void EngineEnv::SetUser(const UnicodeString &inUser)
{
	mUser = inUser;
}
//------------------------------------------------------------------------------

std::string EngineEnv::ToString() const
{
	std::string s("EngineEnv: ");

	//s += mDbManager->GetColId();
	//s += " - ";
	//s += UnicodeString2String(mColShortLib);

	return s;
}
//------------------------------------------------------------------------------
