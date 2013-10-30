/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqSearch.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqSearch.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../util/SearchRecord.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqSearch::XmlReqSearch(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
											 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqSearch;
	mCmdRes = eResSearch;
	mSearchRecord = new SearchRecord;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqSearch::~XmlReqSearch()
{
	delete mSearchRecord;
}
//------------------------------------------------------------------------------

void XmlReqSearch::Clear()
{
	XmlReq::Clear();
	mAction = eXmlCmdNone;
	mTypResRech = 0;
	mNbMotPrev = mNbMotNext = DEF_IntMax;
	mCorpusNum = 0;
	mCorpusName = "";
}
//------------------------------------------------------------------------------

bool XmlReqSearch::Execute()
{
	bool isOk = true;

	mLog.log(eTypLogAction, "Act > XmlReqSearch execute begin");

	OutputHeader();

	/*
	std::string elem;
	std::string attr;
	SearchCrit searchCrit;
	bool isLowerCase = true;
	SearchCritVector searchCritVector;
	SearchCritVector::const_iterator it, itEnd;
	it = mCtSearchCrit->begin();
	itEnd = mCtSearchCrit->end();
	for (; it != itEnd; it++) {
		const SearchCrit &sc = *it;
		searchCrit.mWrd = sc.mWrd;
		searchCrit.mCrit.SetConfigParam(elem, attr, eTypIndexWord, isLowerCase);
		searchCritVector.push_back(sc);
	}
	*/

	// Compute the search a criteria or reload the search id
	isOk &= EngineApiComputeCrit(mEngineEnv, *mSearchRecord);

	EntrySet entrySet;
	if (isOk)
		EngineApiGetSearchResult(entrySet, mSearchRecord->mEntrySet, mMin, mNb, mMax);

	// Output the result
	if (isOk) {
		OutputSearchId();
		OutputPlage();
		Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListDocTexte));
		Printf("%s%s<!-- Total number of results: %u -->\n", DEF_Tab, DEF_Tab, entrySet.size());
		// Iterate the result set and print the search results
		UnicodeString content;
		char bufEntry[DEF_SizeOfEntryScan];
		EntrySet::const_iterator it = entrySet.begin();
		EntrySet::const_iterator itEnd = entrySet.end();
		for (; it != itEnd; ++it) {
			const Entry &e = (*it);
			e.ToStringDocPagePos(bufEntry);
			//isOk &= EngineApiGetDocumentContext(mEngineEnv, content, e);
			content = bufEntry; // Should be the context in the document
			content += " search result context";
			Printf("%s%s<%s>\n", DEF_Tab, DEF_Tab, gXmlTok(eDocTexte));
			Printf("%s%s%s<%s>", DEF_Tab, DEF_Tab, DEF_Tab, gXmlTok(eDocPagPos));
			OutputStr(bufEntry);
			Printf("</%s>\n", gXmlTok(eDocPagPos));
			Printf("%s%s%s<%s>", DEF_Tab, DEF_Tab, DEF_Tab, gXmlTok(eTexte));
			OutputXml(content);
			Printf("</%s>\n", gXmlTok(eTexte));
			Printf("%s%s</%s>\n", DEF_Tab, DEF_Tab, gXmlTok(eDocTexte));
		}
		Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListDocTexte));
	}

	OutputFooter();

	mLog.log(eTypLogAction, "Act > XmlReqSearch execute end");

	return isOk;
}
//------------------------------------------------------------------------------
