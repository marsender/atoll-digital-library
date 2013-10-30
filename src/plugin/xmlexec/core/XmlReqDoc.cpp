/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqDoc.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqDoc.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../util/SearchRecord.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqDoc::XmlReqDoc(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
												 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqDoc;
	mCmdRes = eResDoc;
	mSearchRecord = new SearchRecord;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqDoc::~XmlReqDoc()
{
	delete mSearchRecord;
}
//------------------------------------------------------------------------------

void XmlReqDoc::Clear()
{
	XmlReq::Clear();
	mAction = eXmlCmdNone;
	mName = "";
	mLnkTagSearch = "";
	mIsTabmat = false;
	mToolBar = "";
	//mCtObject->Clear();
}
//------------------------------------------------------------------------------

bool XmlReqDoc::Execute()
{
	bool isOk = true;

	mLog.log(eTypLogAction, "Act > XmlReqDoc execute begin");

  switch (mAction) {
  case eDocPagPos:
    break;
	default:
		mLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect action %d", mAction);
		return false;
	}

	if (mDoc == DEF_IntMax || mPge == DEF_LongMax) {
		mLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect document/page");
		return false;
	}

	OutputHeader();

	// Load the search request if given, and init the hilight vector
	IntVector hilightVector;
	if (!mSearchRecord->mSearchId.empty()) {
		// Get the full search result, in order to be able to fill the hilight vector
		isOk &= EngineApiComputeCrit(mEngineEnv, *mSearchRecord);
		if (!isOk) {
			mLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect search request id");
			return false;
		}
		EntrySet::const_iterator it, itEnd;
		it = mSearchRecord->mEntrySet.begin();
		itEnd = mSearchRecord->mEntrySet.end();
		for (; it != itEnd; ++it) {
			const Entry &e = *it;
			if (e.mUiEntDoc < mDoc || e.mUlEntPge < mPge)
				continue;
			if (e.mUiEntDoc > mDoc || e.mUlEntPge > mPge)
				break;
			hilightVector.push_back(e.mUlEntPos);
		}
	}

	// Get the document page
	Entry e(mDoc, mPge, mPos);
	UnicodeString content;
	isOk &= EngineApiGetDocumentContent(mEngineEnv, content, e, hilightVector);

	if (isOk) {
		Printf("%s<%s>\n", DEF_Tab, gXmlTok(eDocTexte));
		char bufEntry[DEF_SizeOfEntryScan];
		e.ToStringDocPagePos(bufEntry);
		Printf("%s%s<%s>%s</%s>\n", DEF_Tab, DEF_Tab, gXmlTok(eDocPagPos), bufEntry, gXmlTok(eDocPagPos));
		Printf("<%s>", gXmlTok(eTexte));
		OutputXml(content);
		Printf("</%s>\n", gXmlTok(eTexte));
		Printf("%s</%s>\n", DEF_Tab, gXmlTok(eDocTexte));
	}

	OutputFooter();

	mLog.log(eTypLogAction, "Act > XmlReqDoc execute end");

	return isOk;
}
//------------------------------------------------------------------------------
