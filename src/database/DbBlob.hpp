/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

DbBlob.hpp

*******************************************************************************/

#ifndef __DbBlob_HPP
#define __DbBlob_HPP
//------------------------------------------------------------------------------

#include "DbWrapper.hpp"
//------------------------------------------------------------------------------

// Common::BinaryBuffer declaration
namespace Common
{
	class BinaryBuffer;
}
//------------------------------------------------------------------------------

namespace Atoll
{
//! Database blob storage: key to binary buffer
/**
	Logic:
		- Stores elements formed by the combination of a key and an associated value
		- Keys are stored as std::strings
		- Value are binary data witch can store anything
*/
class DbBlob : public DbWrapper
{
private:
	int32_t mKeySize;

	// No need for copy and assignment
	DbBlob(const DbBlob &);
	DbBlob &operator=(const DbBlob &);

public:
	//! Database constructor. DbName is the map name
	DbBlob(const DbEnv *inEnv, DbTxn *inTxn,
			const std::string &inDbName, u_int32_t inPageSize,
			u_int32_t inFlags, bool inClearOnOpen, bool inRemoveOnClose = false);
	virtual ~DbBlob();

	//! Open the database
	int Open(DbTxn *inTxn, u_int32_t inFlags, int inMode);

	bool AddElement(const std::string &inKey, const Common::BinaryBuffer &inBinaryBuffer);
	bool GetElement(const std::string &inKey, Common::BinaryBuffer &outBinaryBuffer);
	bool DelElement(const std::string &inKey);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
