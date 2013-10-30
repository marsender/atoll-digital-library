/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqInfo.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqInfo.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../common/UnicodeUtil.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqInfo::XmlReqInfo(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
											 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqInfo;
	mCmdRes = eResInfo;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqInfo::~XmlReqInfo()
{
}
//------------------------------------------------------------------------------

void XmlReqInfo::Clear()
{
	XmlReq::Clear();

	mIsLogFile = false;
	mIsTraceFile = false;
	mIsOutputFile = false;
	mIsCol = false;
	mIsDbHome = false;
	mIsUser = false;
	mIsVersion = false;

	mOutputFile = "";
}
//------------------------------------------------------------------------------

bool XmlReqInfo::Execute()
{
	bool isOk = true;

	mLog.log(eTypLogAction, "Act > XmlReqInfo execute begin");

	OutputHeader();

	// Engine log file name
	if (mIsLogFile) {
		const char *elem = gXmlTok(eLogFile);
		const char *fileName = Atoll::GetLogFileName();
		Printf("%s<%s>%s</%s>\n", DEF_Tab, elem, fileName, elem);
		mIsLogFile = false;
	}

	// Engine trace file name
	if (mIsTraceFile) {
		const char *elem = gXmlTok(eTraceFile);
		const char *fileName = Atoll::GetTraceFileName();
		Printf("%s<%s>%s</%s>\n", DEF_Tab, elem, fileName, elem);
		mIsTraceFile = false;
	}

	// Engine output file
	if (mIsOutputFile) {
		const char *elem = gXmlTok(eOutFile);
		Printf("%s<%s>%s</%s>\n", DEF_Tab, elem, mOutputFile.c_str(), elem);
		mIsOutputFile = false;
	}

	// Berkeley Db home directory
	if (mIsDbHome) {
		const char *elem = gXmlTok(eDbHome);
		Printf("%s<%s>", DEF_Tab, elem);
		const char *dbHome = mEngineEnv.GetDbHome();
		if (dbHome)
			OutputStr(dbHome);
		Printf("</%s>\n", elem);
		mIsDbHome = false;
	}

	// Collection id
	if (mIsCol) {
		Printf("%s<%s %s=\"", DEF_Tab, gXmlTok(eCol), gXmlTok(eColId));
		OutputStr(mEngineEnv.GetColId());
		OutputStr("\" />\n");
		mIsCol = false;
	}

	// User
	if (mIsUser) {
		const char *elem = gXmlTok(eUser);
		Printf("%s<%s>", DEF_Tab, elem);
		OutputStr(mEngineEnv.GetUser());
		Printf("</%s>\n", elem);
		mIsUser = false;
	}

	// Version
	if (mIsVersion) {
		const char *elem = gXmlTok(eVersion);
		const char *version = Atoll::GetFullVersion();
		Printf("%s<%s>%s</%s>\n", DEF_Tab, elem, version, elem);
		mIsVersion = false;
	}

	OutputFooter();

	mLog.log(eTypLogAction, "Act > XmlReqInfo execute end");

	return isOk;
}
//------------------------------------------------------------------------------
