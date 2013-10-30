/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfig.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerConfig.hpp"
#include "IndexerConfigNode.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../parser/Parser.hpp"
#include "../parser/IndexerConfigHandler.hpp"
#include <sstream> // For std::ostringstream
#include <memory> // for std::auto_ptr
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

enum eTypToken {
	eTypTokenNone,
	eTypTokenIndexerConfig,
	eTypTokenConstant,
	eTypTokenAction,
	eTypTokenParam,
	eTypTokenEnd
};

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > IndexerConfig: %s", x); }
//------------------------------------------------------------------------------

IndexerConfig::IndexerConfig()
{
	mNbError = 0;
	mIsActionAddConfigParam = false;
	mXercesNodeVector = new XercesNodeVector;

	// Valid elements
	mTokenMap["atoll-indexer-config"] = eTypTokenIndexerConfig;
	mTokenMap["constant"] = eTypTokenConstant;
	mTokenMap["action"] = eTypTokenAction;
	mTokenMap["param"] = eTypTokenParam;

	std::string emptyString;
	mAttrConfigNode = new IndexerConfigNode(emptyString);

	// Optional cache
	// This cache doesn't seem to be very usefull in term of speed
	mIndexerItemVectorMapCache = new IndexerItemVectorMap();

	//IndexerConfigParam configParam;
	//bool isLowerCase = true;

	// Default config
	/*
	configParam.SetConfigParam("", "", eTypIndexWord, isLowerCase);
	AddConfigParam(configParam);
	*/

	/*
	bool isTest = true;
	if (isTest) {
		configParam.SetConfigParam("product", "", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("item", "", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("item", "", eTypIndexField, isLowerCase);
		AddConfigParam(configParam);

		// Specific element and attribute
		configParam.SetConfigParam("not", "id", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("b", "", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("e", "", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("sku", "", eTypIndexInterv, isLowerCase);
		AddConfigParam(configParam);

		configParam.SetConfigParam("indexer_test", "id", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);
		configParam.SetConfigParam("indexer_test", "id", eTypIndexField, isLowerCase);
		AddConfigParam(configParam);
		configParam.SetConfigParam("indexer_test", "id", eTypIndexInterv, isLowerCase);
		AddConfigParam(configParam);
	}
	else {
		// Don't index some Nuance elems
		//configParam.SetConfigParam("wd", "", eTypIndexNothing, isLowerCase);
		//AddConfigParam(configParam);
		//configParam.SetConfigParam("ln", "", eTypIndexNothing, isLowerCase);
		//AddConfigParam(configParam);
		//configParam.SetConfigParam("br", "", eTypIndexNothing, isLowerCase);
		//AddConfigParam(configParam);
		//configParam.SetConfigParam("ces", "", eTypIndexNothing, isLowerCase);
		//AddConfigParam(configParam);
		configParam.SetConfigParam("Y", "", eTypIndexField, false);
		AddConfigParam(configParam);
		configParam.SetConfigParam("F", "", eTypIndexField, false);
		AddConfigParam(configParam);
		configParam.SetConfigParam("T", "", eTypIndexField, false);
		AddConfigParam(configParam);
		configParam.SetConfigParam("body", "", eTypIndexWord, isLowerCase);
		AddConfigParam(configParam);
		configParam.SetConfigParam("pag", "", eTypIndexWord, true);
		AddConfigParam(configParam);
		configParam.SetConfigParam("pag", "", eTypIndexField, false);
		AddConfigParam(configParam);
		//configParam.SetConfigParam("lb", "", eTypIndexWord, true);
		//AddConfigParam(configParam);

		// Test: index any node with value attribute
		//configParam.SetConfigParam("", "value", eTypIndexWord, true);
		//AddConfigParam(configParam);

		// Todo: index nodes without content like: <tag value="ArcNF01.Tit"/>
		// Todo: index nodes with specific attribute like: <div type="partie">
	}
	*/
}
//------------------------------------------------------------------------------

IndexerConfig::~IndexerConfig()
{
	ClearConfigParamMap();
	ClearConfigNodeMap();

	delete mXercesNodeVector;
	delete mAttrConfigNode;

	// Delete the cache
	if (mIndexerItemVectorMapCache) {
		IndexerItem *item;
		IndexerItemVector::iterator it, itEnd;
		IndexerItemVectorMap::iterator itCache, itCacheEnd;
		itCache = mIndexerItemVectorMapCache->begin();
		itCacheEnd = mIndexerItemVectorMapCache->end();
		for (; itCache != itCacheEnd; ++itCache) {
			// Delete the indexer item vector
			IndexerItemVector *ivCache = itCache->second;
			if (ivCache == NULL)
				continue;
			for (it = ivCache->begin(), itEnd = ivCache->end(); it != itEnd; ++it) {
				item = *it;
				delete item;
			}
			delete ivCache;
		}
		delete mIndexerItemVectorMapCache;
	}
}
//------------------------------------------------------------------------------

void IndexerConfig::LoadFromFile(const std::string &inFileName)
{
	bool isOk;

	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	std::auto_ptr<Parser> xercesParser(new Parser(SAX2XMLReader::Val_Auto, catalogFile, eTypHandlerIndexerConfig));
	// Get the handler
	IndexerConfigHandler *handler = static_cast<IndexerConfigHandler *>(xercesParser->GetHandler());
	handler->SetIndexerConfig(this);
	isOk = xercesParser->XercesParse(inFileName);
	if (!isOk)
		gLog.log(eTypLogError, "Err > IndexerConfig LoadFromFile error: %s", inFileName.c_str());

  AppAssert(isOk);
}
//------------------------------------------------------------------------------

/*
bool IndexerConfig::IsParentToken(int inTypToken)
{
	XercesNodeVector::const_reverse_iterator it, itEnd;
	it = mXercesNodeVector->rbegin();
	itEnd = mXercesNodeVector->rend();
	for (; it != itEnd; ++it) {
		XercesNode *node = *it;
		if (node->GetTypToken() == inTypToken)
			return true;
	}

	return false;
}*/
//------------------------------------------------------------------------------

void IndexerConfig::StartDocument()
{
	ClearConfigParamMap();
	ClearConfigNodeMap();
}
//------------------------------------------------------------------------------

void IndexerConfig::EndDocument(bool inIsException)
{
	if (!mXercesNodeVector->empty()) {
		if (!inIsException) {
			mNbError++;
			gLog.log(eTypLogError, "Err > IndexerConfig EndDocument: elem list not empty");
		}
		mXercesNodeVector->clear();
	}
}
//------------------------------------------------------------------------------

void IndexerConfig::StartNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap)
{
	// Identify the token
	eTypToken typToken;
	StringToIntMap::const_iterator itMap = mTokenMap.find(inElem);
	if (itMap != mTokenMap.end()) {
		typToken = (eTypToken)itMap->second;
	}
	else {
		typToken = eTypTokenNone;
		gLog.log(eTypLogError, "Err > IndexerConfig Xml elem unrecognized: %s", inElem.c_str());
	}

	std::string value;
	StringToUnicodeStringMap::const_iterator it, itEnd;
	switch (typToken) {
	case eTypTokenAction:
		for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
			ConvertUnicodeString2String(value, it->second);
			if (it->first == "type") {
				if (value == "add_config_param")
					mIsActionAddConfigParam = true;
				else {
					gLog.log(eTypLogError, "Err > IndexerConfig Incorrect attribute value: <%s %s=\"%s\">",
						inElem.c_str(), it->first.c_str(), value.c_str());
					mNbError++;
				}
			}
			else {
				gLog.log(eTypLogError, "Err > IndexerConfig Incorrect attribute: <%s %s=\"%s\">",
					inElem.c_str(), it->first.c_str(), value.c_str());
				mNbError++;
			}
		}
		break;
	case eTypTokenParam:
		if (mIsActionAddConfigParam) {
			IndexerConfigParam configParam;
			configParam.mTypIndex = eTypIndexInterv; // Default index type
			configParam.mIsLowerCase = true; // Default index case
			for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
				ConvertUnicodeString2String(value, it->second);
				if (it->first == "name") {
					configParam.mName = value;
				}
				else if (it->first == "elem") {
					configParam.mElem = value;
				}
				else if (it->first == "attr") {
					configParam.mAttr = value;
				}
				else if (it->first == "type") {
					// Index type: word|field|interv|none
					if (value == "word")
						configParam.mTypIndex = eTypIndexWord;
					else if (value == "field")
						configParam.mTypIndex = eTypIndexField;
					else if (value == "interv")
						configParam.mTypIndex = eTypIndexInterv;
					else if (value == "none")
						configParam.mTypIndex = eTypIndexNothing;
					else {
						gLog.log(eTypLogError, "Err > IndexerConfig Incorrect attribute value: <%s %s=\"%s\">",
							inElem.c_str(), it->first.c_str(), value.c_str());
						mNbError++;
					}
				}
				else if (it->first == "case") {
					// Index case: lower|none
					if (value == "lower")
						configParam.mIsLowerCase = true;
					else if (value == "none")
						configParam.mIsLowerCase = false;
					else {
						gLog.log(eTypLogError, "Err > IndexerConfig Incorrect attribute value: <%s %s=\"%s\">",
							inElem.c_str(), it->first.c_str(), value.c_str());
						mNbError++;
					}
				}
				else {
					gLog.log(eTypLogError, "Err > IndexerConfig Incorrect attribute: <%s %s=\"%s\">",
						inElem.c_str(), it->first.c_str(), value.c_str());
					mNbError++;
				}
			}
			AddConfigParam(configParam);
		}
		break;
	default:
		break;
	}

	XercesNode *xercesNode = new XercesNode(inElem, inAttrMap, typToken);
	mXercesNodeVector->push_back(xercesNode);
}
//------------------------------------------------------------------------------

void IndexerConfig::EndNode()
{
	XercesNode *xercesNode = mXercesNodeVector->back();
	mXercesNodeVector->pop_back();

	StringToUnicodeStringMap::const_iterator itAttr;
	switch (xercesNode->GetTypToken()) {
	case eTypTokenAction:
		mIsActionAddConfigParam = false;
		break;
	default:
		break;
	}

	delete xercesNode;
}
//------------------------------------------------------------------------------

void IndexerConfig::AddCharacters(const UChar *inStr, unsigned int inLength) const
{
	// Nothing to do
}
//------------------------------------------------------------------------------

void IndexerConfig::ClearConfigParamMap()
{
	// Delete the configuration map and associated items
	IndexerConfigParam *item;
  IndexerConfigParamMap::iterator it, itEnd;	// Map iterator
  for (it = mConfigParamMap.begin(), itEnd = mConfigParamMap.end(); it != itEnd; ++it) {
		item = it->second;
		delete item;
	}
	mConfigParamMap.clear();
}
//------------------------------------------------------------------------------

void IndexerConfig::ClearConfigNodeMap()
{
	// Delete the configuration map and associated items
	IndexerConfigNode *item;
  IndexerConfigNodeMap::iterator it, itEnd;	// Map iterator
  for (it = mConfigNodeMap.begin(), itEnd = mConfigNodeMap.end(); it != itEnd; ++it) {
		item = it->second;
		delete item;
	}
	mConfigNodeMap.clear();
}
//------------------------------------------------------------------------------

IndexerConfigParam *IndexerConfig::GetConfigParam(const std::string &inIndexName)
{
	IndexerConfigParam *item;
	IndexerConfigParamMap::const_iterator it = mConfigParamMap.find(inIndexName);
	if (it != mConfigParamMap.end()) {
		item = it->second;
		return item;
	}

	return NULL;
}
//------------------------------------------------------------------------------

void IndexerConfig::AddConfigParam(IndexerConfigParam &inConfigParam)
{
	if (!inConfigParam.IsValid()) {
		gLog.log(eTypLogError, "Err > IndexerConfig Incorrect indexer config param: %s",
			inConfigParam.ToString().c_str());
		return;
	}

	// Check if index db name is unique
	const std::string &indexDbName = inConfigParam.GetIndexDbName();
	IndexerConfigParamMap::const_iterator it = mConfigParamMap.find(indexDbName);
	if (it != mConfigParamMap.end()) {
		gLog.log(eTypLogError, "Err > IndexerConfig Indexer config param is not unique: %s",
			inConfigParam.ToString().c_str());
		return;
	}

	// Store the config param in the config param map
	IndexerConfigParam *indexerConfigParam = new IndexerConfigParam(inConfigParam);
	mConfigParamMap[indexDbName] = indexerConfigParam;

	// Init the config node
	IndexerConfigNode *indexerConfigNode;
	std::string &elem = inConfigParam.mElem;
	if (elem.length() == 0) {
		// Attributes config node if no elem is given
		indexerConfigNode = mAttrConfigNode;
	}
	else {
		// Check if a node already exists for this config param
		IndexerConfigNodeMap::const_iterator it = mConfigNodeMap.find(elem);
		if (it != mConfigNodeMap.end()) {
			indexerConfigNode = it->second;
		}
		else {
			indexerConfigNode = new IndexerConfigNode(elem);
			mConfigNodeMap[elem] = indexerConfigNode;
		}
	}

	indexerConfigNode->AddNodeConfigParam(inConfigParam);
}
//------------------------------------------------------------------------------

IndexerItemVector *IndexerConfig::GetNodeIndexerVector(const std::string &inElem,
	const StringToUnicodeStringMap &inAttrMap) const
{
	// Search in the cache if this xml node exist
	// We store only the nodes without attributes actually, but
	// we might serialize the nodes with attributes and cache them too
	bool wantCache = mIndexerItemVectorMapCache && inElem.length() && inAttrMap.size() == 0;
	if (wantCache) {
		IndexerItemVectorMap::const_iterator itCache;
		itCache = mIndexerItemVectorMapCache->find(inElem);
		if (itCache != mIndexerItemVectorMapCache->end()) {
			const IndexerItemVector *ivCache = itCache->second;
			if (ivCache == NULL)
				return NULL;
			// Copy the cache in a new vector
			IndexerItem *newItem;
			const IndexerItem *item;
			IndexerItemVector *ivNew = new IndexerItemVector();
			IndexerItemVector::const_iterator it, itEnd;
			for (it = ivCache->begin(), itEnd = ivCache->end(); it != itEnd; ++it) {
				item = *it;
				newItem = new IndexerItem(item->mIndexDbName, item->mTypIndex, item->mIsLowerCase);
				ivNew->push_back(newItem);
			}
			return ivNew;
		}
	}

	const IndexerConfigNode *item = NULL;
	// Get the config for this node
	IndexerConfigNodeMap::const_iterator it = mConfigNodeMap.find(inElem);
	if (it != mConfigNodeMap.end()) {
		// Check element and attribute config
		item = it->second;
	}
	else {
		// Nothing to do if there is no attributes
		if (inAttrMap.size() == 0)
			return NULL;
		// Check attribute config only
		item = mAttrConfigNode;
	}

	// Create an indexer vector for the xml node, or nothing (and return NULL) if there is nothing to index
	IndexerItemVector *indexerItemVector = item->CreateIndexerItemVector(inElem, inAttrMap);

	// Add the result in the cache
	if (wantCache) {
		IndexerItemVector *ivNew = NULL;
		if (indexerItemVector) {
			// Copy the vector in the cache
			IndexerItem *newItem;
			const IndexerItem *item;
			ivNew = new IndexerItemVector();
			IndexerItemVector::const_iterator it, itEnd;
			for (it = indexerItemVector->begin(), itEnd = indexerItemVector->end(); it != itEnd; ++it) {
				item = *it;
				newItem = new IndexerItem(item->mIndexDbName, item->mTypIndex, item->mIsLowerCase);
				ivNew->push_back(newItem);
			}
		}
		(*mIndexerItemVectorMapCache)[inElem] = ivNew;
	}

	return indexerItemVector;
}
//------------------------------------------------------------------------------
