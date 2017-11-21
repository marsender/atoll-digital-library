/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigNode.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerConfigNode.hpp"
#include "IndexerConfigParam.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > IndexerConfigNode: %s", x); }
//------------------------------------------------------------------------------

IndexerConfigNode::IndexerConfigNode(const std::string &inElem)
{
	mElem = inElem;
}
//------------------------------------------------------------------------------

IndexerConfigNode::~IndexerConfigNode()
{
	// Delete the elem configuration map and associated items
	{
		IndexerConfigParam *item;
		IndexerConfigParamMultiMap::const_iterator it, itEnd;	// MultiMap iterator
		for (it = mConfigMultiMap.begin(), itEnd = mConfigMultiMap.end(); it != itEnd; ++it) {
			item = it->second;
			delete item;
		}
		mConfigMultiMap.clear();
	}
}
//------------------------------------------------------------------------------

void IndexerConfigNode::AddNodeConfigParam(const IndexerConfigParam &inConfigParam)
{
	AppAssert(mElem == inConfigParam.mElem);

	if (inConfigParam.mTypIndex == eTypIndexNothing)
		return;

	std::string mapKey = GetMapKey(inConfigParam.mElem, inConfigParam.mAttr);

	// Todo ?: check if config doesn't already exist in the multi-map

	IndexerConfigParam *item = new IndexerConfigParam(inConfigParam);
	mConfigMultiMap.insert(std::pair<std::string, IndexerConfigParam *>(mapKey, item));
}
//------------------------------------------------------------------------------

std::string IndexerConfigNode::GetMapKey(const std::string &inElem, const std::string &inAttr) const
{
	std::string key(inElem);
	if (inAttr.length()) {
		key += " ";
		key += inAttr;
	}

	return key;
}
//------------------------------------------------------------------------------

IndexerItemVector *IndexerConfigNode::CreateIndexerItemVector(const std::string &inElem,
	const Common::StringToUnicodeStringMap &inAttrMap) const
{
	size_t size;

	size = mConfigMultiMap.size();
	if (size == 0)
		return NULL;

	//gLog.logWithTime(eTypLogAction, "Act > Elem: %s", inElem.c_str());

	std::string emptyString;
	IndexerItemVector *indexerItemVector = NULL;
	// Add indexer items for the elem if it has a config
	if (mElem.length())
		AddIndexerItems(indexerItemVector, inElem, emptyString);

	// Add indexer items for the attributes
	size = inAttrMap.size();
	if (size) {
		const std::string *attr;
		StringToUnicodeStringMap::const_iterator it, itEnd;
		for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
			attr = &it->first;
			// Add indexer items for the elem / attribute if it has a config
			if (mElem.length())
				AddIndexerItems(indexerItemVector, inElem, *attr);
			// Add indexer items for the attribute if it has a config
			AddIndexerItems(indexerItemVector, emptyString, *attr);
		}
	}

	// Return a newly created indexer vector for the xml node, or null if there is nothing to index
	return indexerItemVector;
}
//------------------------------------------------------------------------------

void IndexerConfigNode::AddIndexerItems(IndexerItemVector *&ioIndexerItemVector,
	const std::string &inElem, const std::string &inAttr) const
{
	bool wantTrace = false;

	// Get the key
	std::string mapKey = GetMapKey(inElem, inAttr);

	// Get the range in the multi-map
	IndexerItem *item = NULL;
	std::pair<IndexerConfigParamMultiMap::const_iterator, IndexerConfigParamMultiMap::const_iterator> range;
	range = mConfigMultiMap.equal_range(mapKey);

	// Search in the map if the elem / attribute has one or more configs
	IndexerConfigParamMultiMap::const_iterator it = range.first;
	for (; it != range.second; ++it) {
		IndexerConfigParam *config = it->second;
		//if (config->mTypIndex != eTypIndexNot) {
			const std::string &indexDbName = config->GetIndexDbName();
			item = new IndexerItem(indexDbName, config->mTypIndex, config->mIsLowerCase);
			if (ioIndexerItemVector == NULL)
				ioIndexerItemVector = new IndexerItemVector();
			ioIndexerItemVector->push_back(item);
			if (wantTrace) {
				Trace("IndexerConfigNode::Add indexer item vector %s", item->ToString().c_str());
			}
		//}
	}
}
//------------------------------------------------------------------------------
