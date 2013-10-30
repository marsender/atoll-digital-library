/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColManager.hpp

*******************************************************************************/

#ifndef __ColManager_HPP
#define __ColManager_HPP
//------------------------------------------------------------------------------

#include "../common/Containers.hpp"
#include "../util/ColMeta.hpp"
//------------------------------------------------------------------------------

class DbEnv; // No namespace

namespace Atoll
{
class Collection;
class DbMap;
class DbColMeta;
class DbManager;

//! Application object that holds the collections
/**
	Logic:
		- Holds the list of all the collections that can be accessed
		- Give collections details
*/
class DEF_Export ColManager
{
public:
	ColManager(const DbEnv *inEnv);
	~ColManager();

	//! Remove all stored collections
	bool ClearColManager();

	//! Add a collection. Returns the new collection id
	bool AddCollection(UnicodeString &outColId, const ColMeta &inColMeta);
	//! Remove a collection
	bool RemoveCollection(const UnicodeString &inColId);
	//! Get a collection number (DEF_ShortMax if no col)
	unsigned int GetCollectionNumber(const UnicodeString &inColId);
	//! Get the metadata record for a collection. The metadata field mColId must be set.
	bool GetCollectionMeta(ColMeta &outColMeta);
	//! Set a collection metadata record
	bool SetCollectionMeta(const ColMeta &inColMeta);

	//! Get the number of collections in the database
	unsigned long GetCollectionCount();

	//! Get the list of all collections metadata records
	bool GetColMetaVector(ColMetaVector &outVector);

	//! Open a collection and get it's pointer
	Collection *OpenCollection(const UnicodeString &inColId);

	std::string ToString() const;

private:
	DbManager *mDbManager; //!< Unique access to all databases
	Common::UnicodeStringToIntMap *mColMap; //!< Map of all existing collections
	DbColMeta *mDbColMeta; //!< Database of all existing collections metadata
	DbMap *mDbColMetaMap; //!< Database of the collection map
	Collection *mCollection; //!< Current collection

	//! Open the collection manager
	bool OpenColManager();
	//! Close the collection manager
	void CloseColManager();

	//! Check the container sizes
	bool CheckSizes();
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const ColManager &inColManager);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
