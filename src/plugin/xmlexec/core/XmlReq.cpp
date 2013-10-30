/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReq.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReq.hpp"
#include "../../../common/UnicodeUtil.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../util/SearchRecord.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

#define DEF_BufSize      500   // Maximum size of a printf message
#define DEF_EmptySearchCritPatern "*?*"	// Empty search criterion
//------------------------------------------------------------------------------

using namespace Common;
using namespace AtollPluginXmlExec;

XmlReq::XmlReq(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
							 Atoll::EngineEnv &inEngineEnv)
	: mLog(inLog),
	mEngineEnv(inEngineEnv),
	mPluginMessage(inPluginMessage)
{
	mSearchRecord = NULL;
	mBuf = new char[DEF_BufSize];
}
//------------------------------------------------------------------------------

XmlReq::~XmlReq()
{
	delete [] mBuf;
}
//------------------------------------------------------------------------------

void XmlReq::Clear()
{
	mDoc = DEF_IntMax;
	mPge = DEF_LongMax;
	mPos = 0;
	mMin = 0;
	mNb = mMax = DEF_LongMax;
	if (mSearchRecord)
		mSearchRecord->Clear();
	// Raz graphie (texte sans transformation)
	//gAppParam.mAppParamGraphie = eTypGraphieNone;
	// Raz type de client
	//gTypClient = eTypClientNone;
	//gTypClientPhpRelook = false;
	// Raz utilisateur courant
	//mMap(DEF_MYSQL) && !defined(WIN32)
	//	*gUsrName = 0;
	//	*gUsrPass = 0;
	//#endif
	// Raz type d'os
	//gTypOs = eTypOsNone;
	// Raz langue
	//gLangCanonical = "";
}
//------------------------------------------------------------------------------

void XmlReq::ClearSearchCrit()
{
	AppAssert(mSearchRecord);
	mSearchRecord->mSearchCritVector.clear();
}
//------------------------------------------------------------------------------

void XmlReq::AddSearchCrit(const Atoll::SearchCrit &inSearchCrit)
{
	// Check if the search crit is empty
	// For search historics, keep search criterion with empty patern
	if (inSearchCrit.mWrd.isEmpty()
		|| (mCmdReq != eReqUsr && inSearchCrit.mWrd == DEF_EmptySearchCritPatern))
		return;

	AppAssert(mSearchRecord);
	mSearchRecord->mSearchCritVector.push_back(inSearchCrit);
}
//------------------------------------------------------------------------------

void XmlReq::SetSearchId(const std::string &inSearchId)
{
	AppAssert(mSearchRecord);
	mSearchRecord->mSearchId = inSearchId;
}
//------------------------------------------------------------------------------

void XmlReq::Printf(const char *inStr, ...)
{
	va_list ap;
	va_start(ap, inStr);
	if (vsnprintf(mBuf, DEF_BufSize - 1, inStr, ap) == -1) {
		mLog.log(eTypLogError, "Err > XmlReq: Output buffer is too small !");
	}
	va_end(ap);

	mPluginMessage.mMessage += mBuf;
}
//------------------------------------------------------------------------------

void XmlReq::OutputStr(const char *inStr)
{
	mPluginMessage.mMessage += inStr;
}
//------------------------------------------------------------------------------

void XmlReq::OutputStr(const UChar *inStr)
{
	mPluginMessage.mMessage += inStr;
}
//------------------------------------------------------------------------------

void XmlReq::OutputStr(const UnicodeString &inStr)
{
	mPluginMessage.mMessage += inStr;
}
//------------------------------------------------------------------------------

void XmlReq::OutputXml(const char *inStr)
{
	UnicodeString str(inStr);
	UnicodeStringAddXmlEntities(mPluginMessage.mMessage, str.getTerminatedBuffer());
}
//------------------------------------------------------------------------------

void XmlReq::OutputXml(const UChar *inStr)
{
	UnicodeStringAddXmlEntities(mPluginMessage.mMessage, inStr);
}
//------------------------------------------------------------------------------

void XmlReq::OutputXml(const UnicodeString &inStr)
{
	UnicodeString &str = (UnicodeString &)inStr; // Const hack
	UnicodeStringAddXmlEntities(mPluginMessage.mMessage, str.getTerminatedBuffer());
}
//------------------------------------------------------------------------------

void XmlReq::OutputHeader()
{
	UChar buf[50];

	u_sprintf(buf, "<%s>\n", gXmlTok(mCmdRes));

	mPluginMessage.mMessage += buf;
}
//------------------------------------------------------------------------------

void XmlReq::OutputFooter()
{
	UChar buf[50];

	u_sprintf(buf, "</%s>\n\n", gXmlTok(mCmdRes));

	mPluginMessage.mMessage += buf;
}
//------------------------------------------------------------------------------

void XmlReq::OutputSearchId()
{
	if (!mSearchRecord)
		return;

	UChar buf[200];

	u_sprintf(buf, "%s<%s>%s</%s>\n",
		DEF_Tab, gXmlTok(eSearchId), mSearchRecord->mSearchId.c_str(), gXmlTok(eSearchId));

	mPluginMessage.mMessage += buf;
}
//------------------------------------------------------------------------------

void XmlReq::OutputPlage()
{
	UChar buf[200];

	u_sprintf(buf, "%s<%s>\n%s%s<%s>%lu</%s>\n%s%s<%s>%lu</%s>\n%s%s<%s>%lu</%s>\n%s</%s>\n",
		DEF_Tab, gXmlTok(ePlage),
		DEF_Tab, DEF_Tab, gXmlTok(eMin), mMin, gXmlTok(eMin),
		DEF_Tab, DEF_Tab, gXmlTok(eNb), mNb, gXmlTok(eNb),
		DEF_Tab, DEF_Tab, gXmlTok(eMax), mMax, gXmlTok(eMax),
		DEF_Tab, gXmlTok(ePlage));

	mPluginMessage.mMessage += buf;
}
//------------------------------------------------------------------------------
