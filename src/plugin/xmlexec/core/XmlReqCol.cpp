/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReqCol.cpp

*******************************************************************************/

#include "../../../Portability.hpp"
#include "XmlReqCol.hpp"
#include "../../../engine/EngineApi.hpp"
#include "../../../engine/EngineEnv.hpp"
#include "../../../plugin/PluginMessage.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
using namespace AtollPluginXmlExec;

XmlReqCol::XmlReqCol(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
										 Atoll::EngineEnv &inEngineEnv)
	: XmlReq(inLog, inPluginMessage, inEngineEnv)
{
	mCmdReq = eReqCol;
	mCmdRes = eResCol;

	Clear();
}
//------------------------------------------------------------------------------

XmlReqCol::~XmlReqCol()
{
}
//------------------------------------------------------------------------------

void XmlReqCol::Clear()
{
	XmlReq::Clear();
}
//------------------------------------------------------------------------------

void XmlReqCol::OutputDocMeta(const Atoll::DocMeta &inDocMeta, eXmlCmd inCmd, bool inIsOk)
{
	if (!inIsOk) {
		Printf("%s<!-- %s error: [%u] -->\n", DEF_Tab, gXmlTok(inCmd), inDocMeta.mDocNum);
		return;
	}

	switch (inCmd) {
	case eDoc:
	case eGetDoc:
		Printf("%s%s<%s %s=\"%u\" %s=\"%lu\" %s=\"", inCmd == eDoc ? DEF_Tab : "", DEF_Tab, gXmlTok(inCmd),
			gXmlTok(eDocNum), inDocMeta.mDocNum,
			gXmlTok(eCountPge), inDocMeta.mCountPge,
			gXmlTok(eFileName));
		OutputStr(inDocMeta.mFileName.c_str());
		Printf("\" %s=\"", gXmlTok(eDcMetadata));
		OutputStr(inDocMeta.mDcMetadata.c_str());
		Printf("\" %s=\"", gXmlTok(eIndexerConfig));
		OutputStr(inDocMeta.mIndexerConfig.c_str());
		Printf("\" %s=\"", gXmlTok(eRecordBreakerConfig));
		OutputStr(inDocMeta.mRecordBreakerConfig.c_str());
		Printf("\" %s=\"", gXmlTok(eUuid));
		OutputXml(inDocMeta.mUuid);
		Printf("\" %s=\"", gXmlTok(eXsltCode));
		OutputStr(inDocMeta.mXsltCode);
		Printf("\" %s=\"%s", gXmlTok(eXmlValidation), inDocMeta.mXmlValidation ? "true": "false");
		Printf("\" %s=\"%s", gXmlTok(eIsIndexed), inDocMeta.mIsIndexed ? "true": "false");
		OutputStr("\">");
		OutputXml(inDocMeta.mDocTitle);
		Printf("</%s>\n", gXmlTok(inCmd));
		break;
	default:
		Printf("%s<%s %s=\"%u\" />\n", DEF_Tab, gXmlTok(inCmd), gXmlTok(eDocNum), inDocMeta.mDocNum);
		break;
	}
}
//------------------------------------------------------------------------------

void XmlReqCol::OutputFieldMeta(const Atoll::FieldMeta &inFieldMeta, eXmlCmd inCmd, bool inIsOk)
{
	std::string strFieldType;
	UnicodeString content(inFieldMeta.GetFieldId());

	// Get the litteral field type
	inFieldMeta.GetStrFieldType(strFieldType);

	if (!inIsOk) {
		Printf("%s<!-- %s error: [%s - ", DEF_Tab, gXmlTok(inCmd), strFieldType.c_str());
		OutputXml(content);
		OutputStr("] -->\n");
		return;
	}

	Printf("%s%s<%s %s=\"%s\" ", inCmd == eField ? DEF_Tab : "", DEF_Tab, gXmlTok(inCmd),
		gXmlTok(eType), strFieldType.c_str());
	switch (inCmd) {
	case eField:
	case eGetField:
		Printf("%s=\"%u\" ", gXmlTok(eOrder), inFieldMeta.mFieldOrder);
		break;
	default:
		break;
	}
	Printf("%s=\"", gXmlTok(eFieldId));
	OutputXml(content);

	switch (inCmd) {
	case eField:
	case eGetField:
		OutputStr("\">");
		OutputXml(inFieldMeta.mFieldTitle);
		Printf("</%s>\n", gXmlTok(inCmd));
		break;
	default:
		OutputStr("\" />\n");
		break;
	}
}
//------------------------------------------------------------------------------

bool XmlReqCol::ListDoc()
{
	bool isOk;

	Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListDoc));

	// Get the document metadata vector
	DocMetaVector vector;
	isOk = EngineApiGetDocMetaVector(mEngineEnv, vector);
	if (isOk)
		Printf("%s%s<!-- Total number of docs: %u -->\n", DEF_Tab, DEF_Tab, vector.size());
	else
		Printf("%s<!-- ListDoc error -->\n", DEF_Tab);

	// Iterate the vector and print documents metadata
	if (isOk) {
		DocMetaVector::const_iterator it = vector.begin();
		DocMetaVector::const_iterator itEnd = vector.end();
		for (; it != itEnd; ++it) {
			const DocMeta &docMeta = (*it);
			OutputDocMeta(docMeta, eDoc, true);
		}
	}

	Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListDoc));

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::AddDocMeta(const DocMeta &inDocMeta)
{
	bool isOk;

	// Add the document
	unsigned int docNum = 0;
	bool wantIndex = false; // Don't index the document
	isOk = EngineApiAddDocument(docNum, mEngineEnv, inDocMeta, wantIndex);

	// Send the response
	if (isOk) {
		DocMeta docMeta;
		docMeta = inDocMeta;
		docMeta.mDocNum = docNum;
		OutputDocMeta(docMeta, eAddDoc, true);
	}
	else {
		Printf("%s<!-- %s error: [", DEF_Tab, gXmlTok(eAddDoc));
		OutputXml(inDocMeta.mFileName.c_str());
		OutputStr("] -->\n");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::GetDocMeta(Atoll::DocMeta &ioDocMeta)
{
	bool isOk;

	// Get the document
	isOk = EngineApiGetDocument(mEngineEnv, ioDocMeta);

	// Send the response
	OutputDocMeta(ioDocMeta, eGetDoc, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::SetDocMeta(const Atoll::DocMeta &inDocMeta)
{
	bool isOk;

	// Set the document
	isOk = EngineApiSetDocument(mEngineEnv, inDocMeta);

	// Send the response
	OutputDocMeta(inDocMeta, eSetDoc, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::DelDoc(unsigned int inDocNum)
{
	bool isOk;

	// Remove the document
	isOk = EngineApiDelDocument(mEngineEnv, inDocNum);

	if (isOk) {
		Printf("%s<%s %s=\"%u\" />\n", DEF_Tab, gXmlTok(eDelDoc), gXmlTok(eDocNum), inDocNum);
	}
	else {
		Printf("%s<!-- %s error: [%u] -->\n", DEF_Tab, gXmlTok(eDelDoc), inDocNum);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::ListField(unsigned int inTypField)
{
	bool isOk;

	Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListField));

	// Get the field metadata vector
	FieldMetaVector vector;
	isOk = EngineApiGetFieldMetaVector(mEngineEnv, vector, (eTypField)inTypField);
	if (isOk)
		Printf("%s%s<!-- Total number of fields: %u -->\n", DEF_Tab, DEF_Tab, vector.size());
	else
		Printf("%s<!-- ListField error -->\n", DEF_Tab);

	// Iterate the vector and print fields metadata
	if (isOk) {
		FieldMetaVector::const_iterator it = vector.begin();
		FieldMetaVector::const_iterator itEnd = vector.end();
		for (; it != itEnd; ++it) {
			const FieldMeta &fieldMeta = (*it);
			OutputFieldMeta(fieldMeta, eField, true);
		}
	}

	Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListField));

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::AddFieldMeta(const FieldMeta &inFieldMeta)
{
	bool isOk;

	// Add the field
	isOk = EngineApiAddField(mEngineEnv, inFieldMeta);

	// Send the response
	OutputFieldMeta(inFieldMeta, eAddField, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::GetFieldMeta(Atoll::FieldMeta &ioFieldMeta)
{
	bool isOk;

	// Get the field
	isOk = EngineApiGetField(mEngineEnv, ioFieldMeta);

	// Send the response
	OutputFieldMeta(ioFieldMeta, eGetField, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::SetFieldMeta(const Atoll::FieldMeta &inFieldMeta)
{
	bool isOk;

	// Set the field
	isOk = EngineApiSetField(mEngineEnv, inFieldMeta);

	// Send the response
	OutputFieldMeta(inFieldMeta, eSetField, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::DelFieldMeta(const Atoll::FieldMeta &inFieldMeta)
{
	bool isOk;

	// Remove the field
	isOk = EngineApiDelField(mEngineEnv, inFieldMeta);

	OutputFieldMeta(inFieldMeta, eDelField, isOk);

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::IndexDoc(unsigned int inDocNum)
{
	bool isOk;

	// Index the document and store it's content
	isOk = EngineApiIndexDocument(mEngineEnv, inDocNum);

	if (isOk) {
		Printf("%s<%s %s=\"%u\" />\n", DEF_Tab, gXmlTok(eIndexDoc), gXmlTok(eDocNum), inDocNum);
	}
	else {
		Printf("%s<!-- %s error: [%u] -->\n", DEF_Tab, gXmlTok(eIndexDoc), inDocNum);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::DelDocIndex(unsigned int inDocNum)
{
	bool isOk;

	// Remove the document index and storage content
	isOk = EngineApiDelDocumentIndex(mEngineEnv, inDocNum);

	if (isOk) {
		Printf("%s<%s %s=\"%u\" />\n", DEF_Tab, gXmlTok(eDelDocIndex), gXmlTok(eDocNum), inDocNum);
	}
	else {
		Printf("%s<!-- %s error: [%u] -->\n", DEF_Tab, gXmlTok(eDelDocIndex), inDocNum);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::Execute()
{
	bool isOk = true;

	//mLog.log(eTypLogAction, "Act > XmlReqCol execute begin");

	//OutputHeader();

	mLog.log(eTypLogError, "Err > XmlReqCol don't need execution");

	//OutputFooter();

	//mLog.log(eTypLogAction, "Act > XmlReqCol execute end");

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlReqCol::ListNdx()
{
	bool isOk;

	Printf("%s<%s>\n", DEF_Tab, gXmlTok(eListNdx));

	// Get the index map
	UnicodeStringToIntMap map;
	isOk = EngineApiGetIndexMap(mEngineEnv, map);
	if (isOk)
		Printf("%s%s<!-- Total number of index: %u -->\n", DEF_Tab, DEF_Tab, map.size());
	else
		Printf("%s<!-- ListNdx error -->\n", DEF_Tab);

	// Iterate the map and print index names and size
	if (isOk) {
		UChar bufOpen[50], bufClose[50];
		u_sprintf(bufOpen, "%s%s<%s>", DEF_Tab, DEF_Tab, gXmlTok(eTexte));
		u_sprintf(bufClose, "</%s>\n", gXmlTok(eTexte));
		UnicodeStringToIntMap::const_iterator it = map.begin();
		UnicodeStringToIntMap::const_iterator itEnd = map.end();
		for (; it != itEnd; ++it) {
			const UnicodeString &str = it->first;
			const unsigned int &value = it->second;
			OutputStr(bufOpen);
			OutputStr(str);
			Printf(" [size=%u]", value);
			OutputStr(bufClose);
		}
	}

	Printf("%s</%s>\n", DEF_Tab, gXmlTok(eListNdx));

	return isOk;
}
//------------------------------------------------------------------------------
