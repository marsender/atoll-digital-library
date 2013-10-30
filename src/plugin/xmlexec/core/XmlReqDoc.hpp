/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqDoc.hpp

*******************************************************************************/

#ifndef __XmlReqDoc_HPP
#define __XmlReqDoc_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace AtollPluginXmlExec
{

//! XmlExec text document query
class XmlReqDoc : public XmlReq
{
private:
	//FichAff *mFa;
public:
	eXmlCmd mAction;
	bool mIsTabmat;
	UnicodeString mName;
	UnicodeString mLnkTagSearch;
	UnicodeString mToolBar;
	//Conteneur < UnicodeString > *mCtObject;

	XmlReqDoc(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqDoc();
	virtual void Clear();
	virtual bool Execute();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
