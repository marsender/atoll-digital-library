/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqCol.hpp

*******************************************************************************/

#ifndef __XmlReqCol_HPP
#define __XmlReqCol_HPP
//------------------------------------------------------------------------------

#include "XmlReq.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{
	class DocMeta;
	class FieldMeta;
}

namespace AtollPluginXmlExec
{

//! XmlExec collection query
class XmlReqCol : public XmlReq
{
private:
	void OutputDocMeta(const Atoll::DocMeta &inDocMeta, eXmlCmd inCmd, bool inIsOk);
	void OutputFieldMeta(const Atoll::FieldMeta &inFieldMeta, eXmlCmd inCmd, bool inIsOk);

public:
	XmlReqCol(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReqCol();
	virtual void Clear();
	virtual bool Execute();

	bool ListDoc();
	bool AddDocMeta(const Atoll::DocMeta &inDocMeta);
	bool GetDocMeta(Atoll::DocMeta &ioDocMeta);
	bool SetDocMeta(const Atoll::DocMeta &inDocMeta);
	bool DelDoc(unsigned int inDocNum);
	bool IndexDoc(unsigned int inDocNum);
	bool DelDocIndex(unsigned int inDocNum);

	bool ListField(unsigned int inTypField);
	bool AddFieldMeta(const Atoll::FieldMeta &inFieldMeta);
	bool GetFieldMeta(Atoll::FieldMeta &ioFieldMeta);
	bool SetFieldMeta(const Atoll::FieldMeta &inFieldMeta);
	bool DelFieldMeta(const Atoll::FieldMeta &inFieldMeta);

	bool ListNdx();
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
