/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqIndex.hpp

*******************************************************************************/

#ifndef __XmlReqIndex_HPP
#define __XmlReqIndex_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! XmlExec index query
class XmlReqIndex : public XmlReq
{
private:
	UnicodeString mIdCrit; //!< Index name

public:
	eXmlCmd mAction;
  eXmlCmd mTypIndex; //!< Index type: index or list ?
  eXmlCmd mTypSort;		//!< Index sort type ?
  UnicodeString mWord; //!< Index position word
	long mCorpusNum;
	UnicodeString mCorpusName;

	XmlReqIndex(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqIndex();

	virtual void Clear();
	void SetIndexCrit(const UnicodeString &inId); //!< Set the index name

	virtual bool Execute();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
