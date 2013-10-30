/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqDb.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqDb.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "../../../util/ColMeta.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqDb::XmlReqDb(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
										 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqDb;
	mCmdRes = eResDb;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqDb::~XmlReqDb()
{
}
//------------------------------------------------------------------------------

void XmlReqDb::Clear()
{
	XmlReq::Clear();
}
//------------------------------------------------------------------------------

void XmlReqDb::OutputColMeta(const ColMeta &inColMeta, eXmlCmd inCmd)
{
	switch (inCmd) {
	case eCol:
	case eGetCol:
		Printf("%s%s<%s %s=\"", inCmd == eCol ? DEF_Tab : "", DEF_Tab, gXmlTok(inCmd), gXmlTok(eColId));
		OutputStr(inColMeta.mColId);
		Printf("\" %s=\"", gXmlTok(eColTitle));
		OutputXml(inColMeta.mColTitle);
		Printf("\" %s=\"", gXmlTok(eColSalt));
		OutputXml(inColMeta.mColSalt);
		Printf("\">");
		OutputXml(inColMeta.mColDesc);
		Printf("</%s>\n", gXmlTok(inCmd));
		break;
	default:
		Printf("%s<%s %s=\"", DEF_Tab, gXmlTok(inCmd), gXmlTok(eColId));
		OutputStr(inColMeta.mColId);
		Printf("\" />\n");
		break;
	}
}
//------------------------------------------------------------------------------

bool XmlReqDb::ListCol()
{
	bool isOk;

	Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListCol));

	// Get the collection metadata vector
	ColMetaVector vector;
	isOk = EngineApiGetColMetaVector(mEngineEnv, vector);
	if (isOk)
		Printf("%s%s<!-- Total number of cols: %u -->\n", DEF_Tab, DEF_Tab, vector.size());
	else
		Printf("%s<!-- ListCol error -->\n", DEF_Tab);

	// Iterate the vector and print collection metadata
	if (isOk) {
		ColMetaVector::const_iterator it = vector.begin();
		ColMetaVector::const_iterator itEnd = vector.end();
		for (; it != itEnd; ++it) {
			const ColMeta &colMeta = (*it);
			OutputColMeta(colMeta, eCol);
		}
	}

	Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListCol));

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqDb::AddColMeta(const ColMeta &inColMeta)
{
	bool isOk;
	UnicodeString colId;

	// Add the collection
	isOk = EngineApiAddCollection(colId, mEngineEnv, inColMeta);

	// Send the response
	if (isOk) {
		ColMeta colMeta;
		colMeta.mColId = colId;
		OutputColMeta(colMeta, eAddCol);
	}
	else {
		Printf("%s<!-- AddCol error: [", DEF_Tab);
		OutputStr(colId);
		Printf("] -->\n");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqDb::GetColMeta(ColMeta &ioColMeta)
{
	bool isOk;

	// Get the collection
	isOk = EngineApiGetCollection(mEngineEnv, ioColMeta);

	// Send the response
	if (isOk) {
		OutputColMeta(ioColMeta, eGetCol);
	}
	else {
		Printf("%s<!-- GetCol error: [", DEF_Tab);
		OutputStr(ioColMeta.mColId);
		Printf("] -->\n");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqDb::SetColMeta(const ColMeta &inColMeta)
{
	bool isOk;

	// Update the collection
	isOk = EngineApiSetCollection(mEngineEnv, inColMeta);

	// Send the response
	if (isOk) {
		OutputColMeta(inColMeta, eSetCol);
	}
	else {
		Printf("%s<!-- SetCol error: [", DEF_Tab);
		OutputStr(inColMeta.mColId);
		Printf("] -->\n");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqDb::DelCol(const UnicodeString &inColId)
{
	bool isOk;

	// Remove the collection
	isOk = EngineApiDelCollection(mEngineEnv, inColId);

	if (isOk) {
		Printf("%s<%s %s=\"", DEF_Tab, gXmlTok(eDelCol), gXmlTok(eColId));
		OutputStr(inColId);
		Printf("\" />\n");
	}
	else {
		Printf("%s<!-- DelCol error: [", DEF_Tab);
		OutputStr(inColId);
		Printf("] -->\n");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqDb::Execute()
{
	bool isOk = true;

	//mLog.log(eTypLogAction, "Act > XmlReqDb execute begin");

	//OutputHeader();

	mLog.log(eTypLogError, "Err > XmlReqDb don't need execution");

	/*
	if (mColMetaVector->size()) {
		Printf("%s<%s>\n", DEF_Tab, gXmlTok(eAddCol));
		Printf("%s%s<!-- Number of cols to add: %u -->\n", DEF_Tab, DEF_Tab, mColMetaVector->size());
		// Iterate the collection vector and add the collections
		UChar bufOpen[50], bufClose[50];
		u_sprintf(bufOpen, "%s%s<%s>", DEF_Tab, DEF_Tab, gXmlTok(eTexte));
		u_sprintf(bufClose, "</%s>\n", gXmlTok(eTexte));
		ColMetaVector::const_iterator it = mColMetaVector->begin();
		ColMetaVector::const_iterator itEnd = mColMetaVector->end();
		for (; it != itEnd; ++it) {
			const ColMeta &colMeta = (*it);
			isOk &= EngineApiAddCollection(mEngineEnv, colMeta);
			OutputStr(bufOpen);
			OutputStr(colMeta.mColId);
			OutputStr(bufClose);
		}
		Printf("%s</%s>\n", DEF_Tab, gXmlTok(eAddCol));
		mColMetaVector->clear();
	}
	*/

	//OutputFooter();

	//mLog.log(eTypLogAction, "Act > XmlReqDb execute end");

	return isOk;
}
//------------------------------------------------------------------------------
