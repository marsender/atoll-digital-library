/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqInfo.hpp

*******************************************************************************/

#ifndef __XmlReqInfo_HPP
#define __XmlReqInfo_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! XmlExec information query
class XmlReqInfo : public XmlReq
{
public:
	bool mIsLogFile;
	bool mIsTraceFile;
	bool mIsOutputFile;
	bool mIsDbHome;
	bool mIsCol;
	bool mIsUser;
	bool mIsVersion;
	std::string mOutputFile;

	XmlReqInfo(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqInfo();
	virtual void Clear();
	virtual bool Execute();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
