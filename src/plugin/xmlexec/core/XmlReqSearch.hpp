/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqSearch.hpp

*******************************************************************************/

#ifndef __XmlReqSearch_HPP
#define __XmlReqSearch_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! XmlExec search query
class XmlReqSearch : public XmlReq
{
public:
	eXmlCmd mAction;
	int mNbMotPrev, mNbMotNext;
	long mCorpusNum;
	UnicodeString mCorpusName;

	XmlReqSearch(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqSearch();
	virtual void Clear();
	virtual bool Execute();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
