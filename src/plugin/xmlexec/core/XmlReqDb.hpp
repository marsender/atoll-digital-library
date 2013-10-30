/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqDb.hpp

*******************************************************************************/

#ifndef __XmlReqDb_HPP
#define __XmlReqDb_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{
	class ColMeta;
}

namespace AtollPluginXmlExec
{

//! XmlExec database query
class XmlReqDb : public XmlReq
{
private:
	void OutputColMeta(const Atoll::ColMeta &inColMeta, eXmlCmd inCmd);

public:
	XmlReqDb(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqDb();
	virtual void Clear();
	virtual bool Execute();

	bool ListCol();
	bool AddColMeta(const Atoll::ColMeta &inColMeta);
	bool GetColMeta(Atoll::ColMeta &ioColMeta);
	bool SetColMeta(const Atoll::ColMeta &inColMeta);
	bool DelCol(const UnicodeString &inColId);
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
