/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

ColManager.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "ColManager.hpp"
#include "Collection.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbMap.hpp"
#include "../database/DbColMeta.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

#define DEF_ColManager "colmanager"
#define DEF_ColMetaDbName "colmeta"
#define DEF_ColMapName "colmap"
#define DEF_Log(x) { gLog.log(eTypLogError, "Err > ColManager: %s", x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const ColManager &inColManager)
{
	inStream << inColManager.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

ColManager::ColManager(const DbEnv *inEnv)
	:mCollection(NULL)
{
	mDbManager = new DbManager(inEnv, DEF_ColManager);
	mColMap = new UnicodeStringToIntMap;

	UnicodeString dbName(DEF_ColMetaDbName);
	mDbColMeta = mDbManager->GetDbColMeta(dbName);

	OpenColManager();
}
//------------------------------------------------------------------------------

ColManager::~ColManager()
{
	CloseColManager();

	if (mCollection)
		delete mCollection;
	delete mColMap;
	delete mDbManager;
}
//------------------------------------------------------------------------------

bool ColManager::OpenColManager()
{
	bool isOk = true;
	std::string mapName;

	// Empty collection name is not allowed
	AppAssert(!mDbManager->GetColId().isEmpty());

	// Load the collection map
	mapName = DEF_ColMapName;
	mDbColMetaMap = mDbManager->GetDbMap(mapName);
	if (!mDbColMetaMap->LoadMapFromDb(*mColMap))
		isOk = false;

	if (!isOk) {
		DEF_Log("Open error");
	}

	return isOk;
}
//------------------------------------------------------------------------------

void ColManager::CloseColManager()
{
	// Save the collection map to database
	mDbColMetaMap->SaveMapToDb(*mColMap);
	//mDbManager->CloseDbMap(mapName);
}
//------------------------------------------------------------------------------

bool ColManager::CheckSizes(const char *inLogInfo)
{
	bool isOk = true;

	unsigned long count = (unsigned long)mColMap->size();
	if (!(mDbColMetaMap->Count() == count)) {
		gLog.log(eTypLogError, "Err > ColManager: Db collection map size error in '%s' (DbColMetaMap=%lu != ColMap=%lu)", inLogInfo, mDbColMetaMap->Count(), count);
		isOk = false;
	}
	if (!(mDbColMeta->Count() == count)) {
		gLog.log(eTypLogError, "Err > ColManager: Db collection meta size error in '%s' (DbColMetaMap=%lu != ColMap=%lu)", inLogInfo, mDbColMetaMap->Count(), count);
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool ColManager::ClearColManager()
{
	bool isOk = true;
	UnicodeStringToIntMap::const_iterator it;
	UnicodeStringToIntMap::const_iterator itEnd;

	// Remove all individual collection
	it = mColMap->begin();
	itEnd = mColMap->end();
	for (; it != itEnd; ++it) {
		const UnicodeString &key = it->first;
		// Remove the collection content
		Collection *collection = new Collection(mDbManager->GetDbEnv(), key);
		collection->ClearCollection();
		delete collection;
	}

	// Clear the collection metadata records database
	mDbColMeta->clear();

	// Clear the collection map and it's database storage
	mColMap->clear();
	mDbColMetaMap->clear();

	// Check the container sizes
	isOk = CheckSizes("ClearColManager");

	if (!isOk) {
		DEF_Log("Clear error");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool ColManager::AddCollection(UnicodeString &outColId, const ColMeta &inColMeta)
{
	bool isOk = true;

	/*
	if (inColMeta.mColId.isEmpty()) {
		gLog.log(eTypLogError, "Err > ColManager: Try to add an empty collection");
		return false;
	}
	*/

	if (inColMeta.mColTitle.isEmpty()) {
		gLog.log(eTypLogError, "Err > ColManager: Try to add a collection without title");
		return false;
	}

	// Search the first col number available (greatest + 1)
	unsigned int colNum = 1; // Min col number is 1
	UnicodeStringToIntMap::const_iterator it = mColMap->begin();
	UnicodeStringToIntMap::const_iterator itEnd = mColMap->end();
	for (; it != itEnd; ++it) {
		const unsigned int &value = it->second;
		if (value >= colNum)
			colNum = value + 1;
	}

	// Set the collection id
	if (inColMeta.mColId.isEmpty()) {
		// Auto generate the col id from the col number
		UChar buf[50];
		u_sprintf(buf, "col%02u", colNum);
		outColId = buf;
	}
	else {
		// Use the metadata provided
		outColId = inColMeta.mColId;
	}

	// Search in the col map if the collection id already exists
	if (GetCollectionNumber(outColId) != DEF_IntMax) {
		std::string colId = UnicodeString2String(outColId);
		gLog.log(eTypLogError, "Err > ColManager: Try to add an already existing collection: %s", colId.c_str());
		outColId.remove();
		return false;
	}

	// Add the collection metadata record in the database
	ColMeta colMeta;
	colMeta = inColMeta;
	colMeta.mColNum = colNum;
	colMeta.mColId = outColId;
	if (!mDbColMeta->AddColMeta(colMeta)) {
		outColId.remove();
		return false;
	}

	// Store col name and number in the map database
	mDbColMetaMap->AddElement(colMeta.mColId, colNum);

	// Add the collection in the col map
	(*mColMap)[colMeta.mColId] = colNum;

	// Check the container sizes
	isOk = CheckSizes("AddCollection");

	return isOk;
}
//------------------------------------------------------------------------------

bool ColManager::RemoveCollection(const UnicodeString &inColId)
{
	bool isOk = true;

	if (inColId.isEmpty()) {
		gLog.log(eTypLogError, "Err > ColManager: Try to remove an empty collection name");
		return false;
	}

	// Close the collection if it is opened
	if (mCollection && mCollection->GetColId() == inColId) {
		delete mCollection;
		mCollection = NULL;
	}

	// Search in the col map if the collection exists
	unsigned int colNum = GetCollectionNumber(inColId);
	if (colNum == DEF_IntMax) {
		//std::string colId = UnicodeString2String(inColId);
		//gLog.log(eTypLogError, "Err > ColManager: Try to remove a non existing collection: %s", colId.c_str());
		return false;
	}

	// Remove the collection in the col map
	UnicodeStringToIntMap::iterator it1 = mColMap->find(inColId); // Cannot be const_iterator
	if (it1 == mColMap->end()) {
		std::string colId = UnicodeString2String(inColId);
		gLog.log(eTypLogError, "Err > ColManager: Try to remove a non existing map collection: %s", colId.c_str());
		return false;
	}
	mColMap->erase(it1);

	// Remove the collection metadata record
	if (!mDbColMeta->DelColMeta(inColId))
		return false;

	// Remove the collection content
	Collection *collection = new Collection(mDbManager->GetDbEnv(), inColId);
	collection->ClearCollection();
	delete collection;

	// Remove collection from the map database
	mDbColMetaMap->DelElement(inColId);

	// Check the container sizes
	isOk = CheckSizes("RemoveCollection");

	return isOk;
}
//------------------------------------------------------------------------------

unsigned int ColManager::GetCollectionNumber(const UnicodeString &inColId)
{
	UnicodeStringToIntMap::const_iterator it = mColMap->find(inColId);
	if (it == mColMap->end()) {
		return DEF_IntMax;
	}

	return it->second;
}
//------------------------------------------------------------------------------

bool ColManager::GetCollectionMeta(ColMeta &ioColMeta)
{
	if (ioColMeta.mColId.isEmpty()) {
		gLog.log(eTypLogError, "Err > ColManager: Try to get metadata record for an empty collection name");
		return false;
	}

	// Search in the col map if the collection exists
	unsigned int colNum = GetCollectionNumber(ioColMeta.mColId);
	if (colNum == DEF_IntMax) {
		std::string colId = UnicodeString2String(ioColMeta.mColId);
		gLog.log(eTypLogError, "Err > ColManager: Try to get metadata record from an unknown collection: %s", colId.c_str());
		return false;
	}

	// Get the record
	bool isOk = mDbColMeta->GetColMeta(ioColMeta);

	return isOk;
}
//------------------------------------------------------------------------------

bool ColManager::SetCollectionMeta(const ColMeta &inColMeta)
{
	if (inColMeta.mColId.isEmpty()) {
		gLog.log(eTypLogError, "Err > ColManager: Try to set metadata record for an empty collection name");
		return false;
	}

	// Search in the col map if the collection exists
	unsigned int colNum = GetCollectionNumber(inColMeta.mColId);
	if (colNum == DEF_IntMax) {
		std::string colId = UnicodeString2String(inColMeta.mColId);
		gLog.log(eTypLogError, "Err > ColManager: Try to set metadata record from an unknown collection: %s", colId.c_str());
		return false;
	}

	// Set the collection metadata record in the database
	ColMeta colMeta;
	colMeta = inColMeta;
	colMeta.mColNum = colNum;
	if (!mDbColMeta->AddColMeta(colMeta)) {
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

unsigned long ColManager::GetCollectionCount()
{
	unsigned long size = (unsigned long)mColMap->size();

	return size;
}
//------------------------------------------------------------------------------

bool ColManager::GetColMetaVector(ColMetaVector &outVector)
{
	bool isOk = mDbColMeta->GetColMetaVector(outVector);

	return isOk;
}
//------------------------------------------------------------------------------

Collection *ColManager::OpenCollection(const UnicodeString &inColId)
{
	if (mCollection) {
		if (mCollection->GetColId() == inColId)
			return mCollection;
		delete mCollection;
		mCollection = NULL;
	}

	// Search in the col map if the collection exists
	unsigned int colNum = GetCollectionNumber(inColId);
	if (colNum == DEF_IntMax) {
		std::string colId = UnicodeString2String(inColId);
		gLog.log(eTypLogError, "Err > ColManager: Try to open a non existing collection: %s", colId.c_str());
		return NULL;
	}

	mCollection = new Collection(mDbManager->GetDbEnv(), inColId);

	return mCollection;
}
//------------------------------------------------------------------------------

std::string ColManager::ToString() const
{
	std::string s("ColManager: ");

	std::string colId = UnicodeString2String(mDbManager->GetColId());
	s += colId;

	return s;
}
//------------------------------------------------------------------------------
