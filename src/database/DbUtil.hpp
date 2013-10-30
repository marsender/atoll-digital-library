/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbUtil.hpp

*******************************************************************************/

#ifndef __DbUtil_HPP
#define __DbUtil_HPP
//------------------------------------------------------------------------------

class DbEnv;
#include <vector>

namespace Atoll
{

//! Berkeley DB database utilities
class DbUtil
{
public:
	typedef std::vector < std::string > DbList;

	//! Create a database environment
	/**
		This function must be called before any use of the database
	*/
	static DbEnv *StaticCreateEnvironment(const char *inPrefix, const char *inHome,
													 const char *inDataDir);

	//! Delete a database environment
	/**
		This function must be called when the database is closed,
		to clear all databases related ressources
	*/
	static void StaticDeleteEnvironment(DbEnv *&inDbEnv);

	//! Get the list of all databases from the memory pool statistics
	/**
		That's all the databases that may have been created, even if they don't exist anymore.
		One database which has been deleted an created again will appear 2 times
	*/
	static bool StaticGetDbList(DbEnv &inDbEnv, DbList &outDbList);

	//! Remove a list of databases in the environment
	static bool StaticRemoveDbList(DbEnv &inDbEnv, DbList &inDbList);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
