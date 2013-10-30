/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfig.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "RecordBreakerConfig.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../parser/Parser.hpp"
#include "../parser/RecordBreakerConfigHandler.hpp"
#include <sstream> // For std::ostringstream
#include <memory> // for std::auto_ptr
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

enum eTypToken {
	eTypTokenNone,
	eTypTokenRecordBreakerConfig,
	eTypTokenConstant,
	eTypTokenAction,
	eTypTokenParam,
	eTypTokenEnd
};

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > RecordBreakerConfig: %s", x); }
//------------------------------------------------------------------------------

RecordBreakerConfig::RecordBreakerConfig()
{
	mNbError = 0;
	mIsConfigParamAttr = false;
	mIsConfigParamValue = false;
	mIsActionAddConfigParam = false;
	mXercesNodeVector = new XercesNodeVector;

	// Valid elements
	mTokenMap["atoll-recordbreaker-config"] = eTypTokenRecordBreakerConfig;
	mTokenMap["constant"] = eTypTokenConstant;
	mTokenMap["action"] = eTypTokenAction;
	mTokenMap["param"] = eTypTokenParam;

	/* Configuration manuelle
	RecordBreakerConfigParam configParam;
	configParam.SetConfigParam("pb", "", eTypBreakerOpen);
	AddConfigParam(configParam);
	configParam.SetConfigParam("head", "", eTypBreakerClose);
	AddConfigParam(configParam);
	configParam.SetConfigParam("title", "", eTypBreakerStore);
	AddConfigParam(configParam);
	*/
}
//------------------------------------------------------------------------------

RecordBreakerConfig::~RecordBreakerConfig()
{
	ClearConfigParamMap();

	delete mXercesNodeVector;
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::LoadFromFile(const std::string &inFileName)
{
	bool isOk;

	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	std::auto_ptr<Parser> xercesParser(new Parser(SAX2XMLReader::Val_Auto, catalogFile, eTypHandlerRecordBreakerConfig));
	// Get the handler
	RecordBreakerConfigHandler *handler = static_cast<RecordBreakerConfigHandler *>(xercesParser->GetHandler());
	handler->SetRecordBreakerConfig(this);
	isOk = xercesParser->XercesParse(inFileName);
	if (!isOk)
		gLog.log(eTypLogError, "Err > RecordBreakerConfig LoadFromFile error: %s", inFileName.c_str());

  AppAssert(isOk);
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::StartDocument()
{
	ClearConfigParamMap();
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::EndDocument(bool inIsException)
{
	if (!mXercesNodeVector->empty()) {
		if (!inIsException) {
			mNbError++;
			gLog.log(eTypLogError, "Err > RecordBreakerConfig EndDocument: elem list not empty");
		}
		mXercesNodeVector->clear();
	}
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::StartNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap)
{
	// Identify the token
	eTypToken typToken;
	StringToIntMap::const_iterator itMap = mTokenMap.find(inElem);
	if (itMap != mTokenMap.end()) {
		typToken = (eTypToken)itMap->second;
	}
	else {
		typToken = eTypTokenNone;
		gLog.log(eTypLogError, "Err > RecordBreakerConfig Xml elem unrecognized: %s", inElem.c_str());
	}

	std::string value;
	StringToUnicodeStringMap::const_iterator it, itEnd;
	switch (typToken) {
	case eTypTokenAction:
		for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
			ConvertUnicodeString2String(value, it->second);
			if (it->first == "type") {
				if (it->second == "add_config_param")
					mIsActionAddConfigParam = true;
				else {
					gLog.log(eTypLogError, "Err > RecordBreakerConfig Incorrect attribute value: <%s %s=\"%s\">",
						inElem.c_str(), it->first.c_str(), value.c_str());
					mNbError++;
				}
			}
			else {
				gLog.log(eTypLogError, "Err > RecordBreakerConfig Incorrect attribute: <%s %s=\"%s\">",
					inElem.c_str(), it->first.c_str(), value.c_str());
				mNbError++;
			}
		}
		break;
	case eTypTokenParam:
		if (mIsActionAddConfigParam) {
			RecordBreakerConfigParam configParam;
			configParam.mTypBreaker = eTypBreakerNone; // Default breaker type
			for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
				ConvertUnicodeString2String(value, it->second);
				if (it->first == "elem") {
					configParam.mElem = value;
				}
				else if (it->first == "attr") {
					configParam.mAttr = value;
				}
				else if (it->first == "value") {
					configParam.mValue = value;
				}
				else if (it->first == "type") {
					// Breaker type: open|close
					if (value == "open")
						configParam.mTypBreaker = eTypBreakerOpen;
					else if (value == "close")
						configParam.mTypBreaker = eTypBreakerClose;
					else if (value == "store")
						configParam.mTypBreaker = eTypBreakerStore;
					else {
						gLog.log(eTypLogError, "Err > RecordBreakerConfig Incorrect attribute value: <%s %s=\"%s\">",
							inElem.c_str(), it->first.c_str(), value.c_str());
						mNbError++;
					}
				}
				else {
					gLog.log(eTypLogError, "Err > RecordBreakerConfig Incorrect attribute: <%s %s=\"%s\">",
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

void RecordBreakerConfig::EndNode()
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

void RecordBreakerConfig::AddCharacters(const UChar *inStr, unsigned int inLength) const
{
	// Nothing to do
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::ClearConfigParamMap()
{
	// Delete the configuration map and associated items
	RecordBreakerConfigParam *item;
  RecordBreakerConfigParamMap::iterator it, itEnd;	// Map iterator
  for (it = mConfigParamMap.begin(), itEnd = mConfigParamMap.end(); it != itEnd; ++it) {
		item = it->second;
		delete item;
	}
	mConfigParamMap.clear();

	// Raz config param attribute and value
	mIsConfigParamAttr = false;
	mIsConfigParamValue = false;
}
//------------------------------------------------------------------------------

void RecordBreakerConfig::AddConfigParam(RecordBreakerConfigParam &inConfigParam)
{
	RecordBreakerConfigParam *item;
	std::string mapKey = GetMapKey(inConfigParam.mElem, inConfigParam.mAttr, inConfigParam.mValue);

	if (mapKey.length() == 0) {
		DEF_Log("Cannot add empty config param");
		return;
	}

	// Detect if there is a config param with an attribute
	if (inConfigParam.mAttr.length()) {
		if (inConfigParam.mElem.length()) {
			DEF_Log("Cannot add config param attribute with non empty elem");
			return;
		}
		if (inConfigParam.mTypBreaker != eTypBreakerOpen) {
			DEF_Log("Config param attribute can be used only with type=\"open\"");
			return;
		}
		mIsConfigParamAttr = true;
	}

	// Detect if there is a config param with a value
	if (inConfigParam.mValue.length()) {
		if (!inConfigParam.mAttr.length()) {
			DEF_Log("Cannot add config param value with empty attr");
			return;
		}
		if (inConfigParam.mTypBreaker != eTypBreakerOpen) {
			DEF_Log("Config param value can be used only with type=\"open\"");
			return;
		}
		mIsConfigParamValue = true;
	}

	// Check if a node already exists for this config param
	RecordBreakerConfigParamMap::const_iterator it = mConfigParamMap.find(mapKey);
	if (it != mConfigParamMap.end()) {
		DEF_Log("Cannot add duplicate config param");
		return;
	}
	item = new RecordBreakerConfigParam(inConfigParam);
	mConfigParamMap[mapKey] = item;
}
//------------------------------------------------------------------------------

std::string RecordBreakerConfig::GetMapKey(const std::string &inElem) const
{
	std::string key(inElem);

	return key;
}
//------------------------------------------------------------------------------

std::string RecordBreakerConfig::GetMapKey(const std::string &inElem, const std::string &inAttr,
																					 const std::string &inValue) const
{
	std::string key(inElem);
	if (inAttr.length()) {
		key += " ";
		key += inAttr;
		if (inValue.length()) {
			key += "=";
			key += inValue;
		}
	}

	return key;
}
//------------------------------------------------------------------------------

unsigned int RecordBreakerConfig::GetRecordBreakerConfigSize()
{
	unsigned int count = (unsigned int)mConfigParamMap.size();

	return count;
}
//------------------------------------------------------------------------------

eTypBreaker RecordBreakerConfig::GetNodeTypBreaker(const std::string &inElem,
																									 const Common::StringToUnicodeStringMap &inAttrMap) const
{
	std::string mapKey;
	RecordBreakerConfigParam *item;
	eTypBreaker typBreaker;

	/* Done once only in begin document
	if (mConfigParamMap.size() == 0) {
		gLog.log(eTypLogError, "Err > RecordBreakerConfig::GetNodeTypBreaker: Empty record breaker config");
		return eTypBreakerNone;
	}
	*/

	// Look for an element breaker
	mapKey = GetMapKey(inElem);
	RecordBreakerConfigParamMap::const_iterator itFind;
	itFind = mConfigParamMap.find(mapKey);
	if (itFind != mConfigParamMap.end()) {
		item = itFind->second;
		typBreaker = item->mTypBreaker;
		return typBreaker;
	}

	// Look for an attribute breaker
	if (mIsConfigParamAttr && &inAttrMap != NULL && inAttrMap.size()) {
		std::string emptyStr;
		std::string value;
		UnicodeString uStr;
		StringToUnicodeStringMap::const_iterator it = inAttrMap.begin();
		StringToUnicodeStringMap::const_iterator itEnd = inAttrMap.end();
		for (; it != itEnd; ++it) {
			const std::string &attr = it->first;
			uStr = it->second;
			UChar2String(value, uStr.getBuffer(), uStr.length());
			//ConvertUnicodeString2String(value, uStr.getBuffer(), uStr.length());
			for (short pass = 0; pass < 2; pass++) {
				switch (pass) {
				case 0:
					if (!mIsConfigParamValue)
						continue;
					mapKey = GetMapKey(emptyStr, attr, value);
					break;
				case 1:
					mapKey = GetMapKey(emptyStr, attr, emptyStr);
					break;
				}
				itFind = mConfigParamMap.find(mapKey);
				if (itFind != mConfigParamMap.end()) {
					item = itFind->second;
					typBreaker = item->mTypBreaker;
					return typBreaker;
				}
			}
		}
	}

	return eTypBreakerNone;
}
//------------------------------------------------------------------------------
