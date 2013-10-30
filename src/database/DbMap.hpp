/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbMap.hpp

*******************************************************************************/

#ifndef __DbMap_HPP
#define __DbMap_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
#include "../common/Containers.hpp"
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

// Common::BinaryBuffer declaration
namespace Common
{
	class BinaryBuffer;
}
//------------------------------------------------------------------------------

namespace Atoll
{
//! Database map storage: unicode string to binary buffer map
/**
	Logic:
		- Maps are a kind of associative containers that stores elements formed by the combination of a key value and a mapped value
		- Keys are stored as unicode strings
		- Mapped value are binary data witch can store anything
		- Allows to store and retrieve an STL container of type map
*/
class DbMap : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbMap(const DbMap &);
	DbMap &operator=(const DbMap &);

	//bool AddElement(const UnicodeString &inKey, const Common::BinaryBuffer &inBinaryBuffer);
	//bool GetElement(const UnicodeString &inKey, Common::BinaryBuffer &outBinaryBuffer);

public:
	//! Database constructor. DbName is the map name
	DbMap(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbMap();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	bool LoadMapFromDb(Common::UnicodeStringToIntMap &outMap);
	void SaveMapToDb(const Common::UnicodeStringToIntMap &inMap);

	bool AddElement(const UnicodeString &inKey, unsigned int inData);
	bool GetElement(const UnicodeString &inKey, unsigned int &outData);
	bool DelElement(const UnicodeString &inKey);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
