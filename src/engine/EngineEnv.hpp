/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineEnv.hpp

*******************************************************************************/

#ifndef __EngineEnvHPP
#define __EngineEnvHPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

class DbEnv; // No namespace

namespace Atoll
{

//! Engine parameters
/**
	Logic:
		- Holds all the configuration parameters
*/
class DEF_Export EngineEnv
{
private:
	DbEnv *mDbEnv;
	UnicodeString mColId;
	UnicodeString mUser;
	//UnicodeString mCatalog;

public:

	EngineEnv();
	~EngineEnv();

	//! Is the environment valid ?
	bool IsValid() const;

	//! Create the database environment object
	bool CreateDbEnv(const std::string &inDbHome);
	//! Get the database environment object
	const DbEnv *GetDbEnv() const;
	//! Get the database home directory
	const char *GetDbHome();

	//! Get the collection id
	const UnicodeString &GetColId() const;
	//! Set the collection id
	void SetColId(const UnicodeString &inColId);

	//! Get the user
	const UnicodeString &GetUser() const;
	//! Set the user
	void SetUser(const UnicodeString &inUser);

	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const EngineEnv &inEngineEnv);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
