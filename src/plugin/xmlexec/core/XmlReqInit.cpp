/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqInit.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqInit.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqInit::XmlReqInit(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
											 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqInit;
	mCmdRes = eResInit;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqInit::~XmlReqInit()
{
}
//------------------------------------------------------------------------------

void XmlReqInit::Clear()
{
	XmlReq::Clear();
	//mOutFile.remove();
}
//------------------------------------------------------------------------------

bool XmlReqInit::Execute()
{
	bool isOk = true;

	mLog.log(eTypLogAction, "Act > XmlReqInit execute begin");

	if (!mDbHome.empty()) {
		if (!mEngineEnv.CreateDbEnv(mDbHome))
			isOk = false;
	}
	if (!mColId.isEmpty()) {
		mEngineEnv.SetColId(mColId);
	}
	if (!mUser.isEmpty()) {
		mEngineEnv.SetUser(mUser);
	}

	mLog.log(eTypLogAction, "Act > XmlReqInit execute end");

	return isOk;
}
//------------------------------------------------------------------------------
