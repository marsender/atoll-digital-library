/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigParam.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerConfigParam.hpp"
//#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > IndexerConfigParam: %s", x); }
//------------------------------------------------------------------------------

std::ostream& Atoll::operator<<(std::ostream& inStream, const IndexerConfigParam &inConfig)
{
	inStream << inConfig.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

IndexerConfigParam::IndexerConfigParam()
{
	mTypIndex = eTypIndexNone;
	mIsLowerCase = true;
}
//------------------------------------------------------------------------------

IndexerConfigParam::IndexerConfigParam(const IndexerConfigParam &inIndexerConfigParam)
{
	mName = inIndexerConfigParam.mName;
	mElem = inIndexerConfigParam.mElem;
	mAttr = inIndexerConfigParam.mAttr;
	mTypIndex = inIndexerConfigParam.mTypIndex;
	mIsLowerCase = inIndexerConfigParam.mIsLowerCase;
}
//------------------------------------------------------------------------------

IndexerConfigParam::IndexerConfigParam(const std::string &inName, const std::string &inElem,
	const std::string &inAttr, eTypIndex inTypIndex, bool isLowerCase)
{
	mName = inName;
	mElem = inElem;
	mAttr = inAttr;
	mTypIndex = inTypIndex;
	mIsLowerCase = isLowerCase;
}
//------------------------------------------------------------------------------

void IndexerConfigParam::SetConfigParam(const char *inName, const char *inElem,
	const char *inAttr, eTypIndex inTypIndex, bool isLowerCase)
{
	mName = inName;
	mElem = inElem;
	mAttr = inAttr;
	mTypIndex = inTypIndex;
	mIsLowerCase = isLowerCase;
}
//------------------------------------------------------------------------------

void IndexerConfigParam::SetConfigParam(const std::string &inName, const std::string &inElem,
	const std::string &inAttr, eTypIndex inTypIndex, bool isLowerCase)
{
	mName = inName;
	mElem = inElem;
	mAttr = inAttr;
	mTypIndex = inTypIndex;
	mIsLowerCase = isLowerCase;
}
//------------------------------------------------------------------------------

void IndexerConfigParam::Clear()
{
	mName.clear();
	mElem.clear();
	mAttr.clear();
	mTypIndex = eTypIndexNone;
	mIsLowerCase = true;
}
//------------------------------------------------------------------------------

bool IndexerConfigParam::IsValid() const
{
	if (mName.empty()) {
		gLog.log(eTypLogError, "Err > IndexerConfigParam: Empty name");
		return false;
	}

	/* Allow empty elem ?
	if (mElem.empty()) {
		gLog.log(eTypLogError, "Err > IndexerConfigParam: Empty element");
		return false;
	}
	*/

	if (mTypIndex <= eTypIndexNone || mTypIndex >= eTypIndexEnd) {
		gLog.log(eTypLogError, "Err > IndexerConfigParam: Incorrect index type");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

const std::string &IndexerConfigParam::GetIndexDbName() const
{
	return mName;
}
//------------------------------------------------------------------------------

/*
std::string IndexerConfigParam::GetIndexDbName(const std::string &inElem) const
{
	if (!IsValid()) {
		return "";
	}

	// Just the criterion name
	return mName;

	//Older version with config infos
	// Indexer element and attribue
	//std::string str;
	//str += inElem;
	//str += " ";
	//str += (mAttr.length() == 0) ? "~" : mAttr.c_str(); // ~ is used for no attribute

	// Indexer options
	//char buf[50];
	//sprintf(buf, " %d %d", mTypIndex, mIsLowerCase);
	//str += buf;

	//return str;
}
//------------------------------------------------------------------------------
*/

std::string IndexerConfigParam::ToString() const
{
	char buf[50];
	std::string s("IndexerConfigParam: ");

	s += "-";
	s += mName;
	s += "-";
	s += mElem;
	s += "-";
	s += mAttr;
	sprintf(buf, "-%d-%d", mTypIndex, mIsLowerCase);
	s += buf;

	return s;
}
//------------------------------------------------------------------------------
