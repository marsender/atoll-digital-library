/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqInit.hpp

*******************************************************************************/

#ifndef __XmlReqInit_HPP
#define __XmlReqInit_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! XmlExec initialisation query
class XmlReqInit : public XmlReq
{
public:
	std::string mDbHome;
	UnicodeString mColId;
	UnicodeString mUser;

	XmlReqInit(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqInit();
	virtual void Clear();
	virtual bool Execute();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
