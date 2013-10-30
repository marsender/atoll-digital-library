/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqIndex.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqIndex.hpp"
#include "../../../common/UnicodeUtil.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../util/SearchRecord.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

#define DEF_SepTexteIndex "[ndx]"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqIndex::XmlReqIndex(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
											 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqIndex;
	mCmdRes = eResIndex;
	mSearchRecord = new SearchRecord;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqIndex::~XmlReqIndex()
{
	delete mSearchRecord;
}
//------------------------------------------------------------------------------

void XmlReqIndex::Clear()
{
	XmlReq::Clear();
	mAction = eXmlCmdNone;
	mIdCrit = "";
	mTypIndex = eXmlCmdNone;
	mTypSort = eXmlCmdNone;
	mWord = "";
	mCorpusNum = 0;
	mCorpusName = "";
}
//------------------------------------------------------------------------------

void XmlReqIndex::SetIndexCrit(const UnicodeString &inId)
{
	mIdCrit = inId;
}
//------------------------------------------------------------------------------

bool XmlReqIndex::Execute()
{
	bool isOk = true;

	mLog.log(eTypLogAction, "Act > XmlReqIndex execute begin");

  switch (mTypIndex) {
  case eIndex:
  case eListe:
  case eArticle:
    break;
  default:
		mLog.log(eTypLogError, "Err > XmlReqIndex: Incorrect index type %d", mTypIndex);
		return false;
  }

  // Init index sort type
  //eTypDynSort theTypSort;
  switch (mTypSort) {
  case eMotFreq:
    //theTypSort = eDynSortMotOcc;
    break;
  case eFreqCroi:
    //theTypSort = eDynSortOccCroi;
    break;
  case eFreqDecr:
    //theTypSort = eDynSortOccDecr;
    break;
  case eFinMot:
    //theTypSort = eDynSortFinMot;
    break;
  case eNumCroi:
		//theTypSort = eDynSortNumCroi;
    break;
  case eNumDecr:
		//theTypSort = eDynSortNumDecr;
    break;
  default:
		mLog.log(eTypLogError, "Err > XmlReqIndex: Incorrect sort type %d", mTypSort);
		return false;
  }

	// Id crit must hold the index database name
	if (mIdCrit.length() == 0) {
		mLog.log(eTypLogError, "Err > XmlReqIndex: Empty index id crit");
		return false;
	}

	// Check if id crit is a search field
	/* Check is not valid for indexes without search field
	{
		FieldMeta fieldMeta;
		fieldMeta.mTypField = eTypFieldSearch;
		fieldMeta.SetFieldId(mIdCrit);
		if (!EngineApiGetField(mEngineEnv, fieldMeta)) {
			mLog.log(eTypLogWarning, "Wrn > XmlReqIndex: IdCrit is not a search field: %s", mIdCrit.c_str());
		}
	}
	*/

	OutputHeader();

	// Get the index content
	WordIntVector wordIntVector;
	isOk &= EngineApiComputeIndex(mEngineEnv, wordIntVector, *mSearchRecord, mIdCrit, mMin, mWord, mNb, mMax);

	// Output the result
	if (isOk) {
		Printf("%s<%s>", DEF_Tab, gXmlTok(eIdCrit));
		OutputXml(mIdCrit);
		Printf("</%s>\n", gXmlTok(eIdCrit));
		OutputPlage();
		Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListTexte));
		Printf("%s%s<!-- Total number of results: %u -->\n", DEF_Tab, DEF_Tab, wordIntVector.size());
		// Iterate the result set and print the index lines
		WordIntVector::const_iterator it = wordIntVector.begin();
		WordIntVector::const_iterator itEnd = wordIntVector.end();
		for (; it != itEnd; ++it) {
			const WordInt &wordInt = (*it);
			Printf("%s%s<%s>", DEF_Tab, DEF_Tab, gXmlTok(eTexte));
			OutputXml(wordInt.mWrd);
			Printf("%s%lu", DEF_SepTexteIndex, wordInt.mInt);
			Printf("</%s>\n", gXmlTok(eTexte));
		}
		Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListTexte));
	}

	OutputFooter();

	mLog.log(eTypLogAction, "Act > XmlReqIndex execute end");

	return isOk;
}
//------------------------------------------------------------------------------
