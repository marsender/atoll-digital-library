/*******************************************************************************

XmlExecRunner.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XmlExecRunner.hpp"
#include "core/XmlReqCol.hpp"
#include "core/XmlReqDb.hpp"
#include "core/XmlReqDoc.hpp"
#include "core/XmlReqIndex.hpp"
#include "core/XmlReqInit.hpp"
#include "core/XmlReqInfo.hpp"
#include "core/XmlReqSearch.hpp"
#include "../../common/UnicodeUtil.hpp"
#include "../../engine/EngineEnv.hpp"
#include "../../plugin/PluginMessage.hpp"
#include "../../util/ColMeta.hpp"
#include "../../util/DocMeta.hpp"
#include "../../util/Entry.hpp"
#include "../../util/FieldMeta.hpp"
#include "../../util/SearchRecord.hpp"
#include "unicode/ustring.h"
#include <sstream> // For std::ostringstream
//------------------------------------------------------------------------------

#define DEF_PrologXml		  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" // standalone=\"yes\"
#define DEF_PrologDtd		  "<!DOCTYPE atoll-plugin-xmlexec PUBLIC \"-//ATOLL-DIGITAL-LIBRARY//DTD Atoll Plugin XmlExec XML V1.1//EN\"\n" \
	"\t\"http://www.atoll-digital-library.org/public/atoll/dtd/atoll_plugin_xmlexec.dtd\">"
#define DEF_XmlVersion    "1.0"

using namespace AtollPluginXmlExec;
using namespace Atoll;
using namespace Common;

XmlExecRunner::XmlExecRunner(AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog)
	:mLog(inLog),
	mTokenMap(XmlExecCmd::StaticGetMap())
{
	// Init document info
	mNbError = 0;
	mXercesNodeVector = new XercesNodeVector;
	mColMeta = NULL;
	mDocMeta = NULL;
	mFieldMeta = NULL;
	mSearchCrit = new SearchCrit;
	mWantInputBuffer = false;

	// Create info struct
	mInfo = new Info;
	mInfo->mPluginMessage = NULL;
 	mInfo->mEngineEnv = new EngineEnv;
	mInfo->mPluginMessage = inPluginMessage;

	// New document and page initialisation
	ClearDocumentInfo();
}
//------------------------------------------------------------------------------

XmlExecRunner::~XmlExecRunner()
{
	// Delete info struct
	delete mInfo->mEngineEnv;
	delete mInfo;

	delete mXercesNodeVector;
	delete mSearchCrit;
	AppAssert(mColMeta == NULL);
	AppAssert(mDocMeta == NULL);
	AppAssert(mFieldMeta == NULL);
}
//------------------------------------------------------------------------------

/*
bool XmlExecRunner::RunnerInitDocument(const std::string &inFileName)
{
	bool isOk = true;

	//SetInputFile(inFileName);

	if (!FileSystem::FileExists(inFileName)) {
		mLog.log(eTypLogError, "Err > Missing file: %s", inFileName.c_str());
		isOk = false;
	}

	return isOk;
}*/
//------------------------------------------------------------------------------

/*
void XmlExecRunner::SetInputFile(const std::string &inFileName)
{
	std::string str, fileName, ext("xml");

	mInfo->mInputFile = inFileName;
}*/
//------------------------------------------------------------------------------

void XmlExecRunner::SetOutputFile(const std::string &inFileName)
{
	mInfo->mOutputFile = inFileName;
}
//------------------------------------------------------------------------------

void XmlExecRunner::StartDocument()
{
	// Plugin message object must be set or it's impossible to send a response
	AppAssert(mInfo->mPluginMessage);

	//mLog.setHeader(FileSystem::GetFileName(mInfo->mInputFile).c_str());

	// New document initialisation
	ClearDocumentInfo();

	// Output xml declaration
  char buf[200];
  sprintf(buf, "%s\n", DEF_PrologXml);
  OutputText(buf);

	// Output dtd
	bool wantDtd = true;
	if (wantDtd) {
    sprintf(buf, "%s\n\n", DEF_PrologDtd);
    OutputText(buf);
  }

	// Output header
	sprintf(buf, "<%s version=\"%s\"><%s>\n\n", gXmlTok(eAtollXmlExec), DEF_XmlVersion, gXmlTok(eResAll));
  OutputText(buf);
}
//------------------------------------------------------------------------------

void XmlExecRunner::EndDocument(bool inIsException)
{
	// Output footer
	if (!inIsException) {
		char buf[100];
		sprintf(buf, "</%s></%s>\n", gXmlTok(eResAll), gXmlTok(eAtollXmlExec));
		OutputText(buf);
	}

	// Write the message in the output file
	if (!mInfo->mOutputFile.empty()) {
		UFILE *fout;
		if ((fout = u_fopen(mInfo->mOutputFile.c_str(), "w", NULL, "utf-8")) == NULL) {
			std::ostringstream oss;
			oss << "Open file for writing: " << mInfo->mOutputFile;
			DEF_Exception(AppException::FileError, oss.str());
		}
		UnicodeString &message = mInfo->mPluginMessage->mMessage;
		u_file_write(message.getBuffer(), message.length(), fout);
		u_fclose(fout);
	}

	if (!mXercesNodeVector->empty()) {
		if (!inIsException) {
			mNbError++;
			mLog.log(eTypLogError, "Err > RunnerEndDocument: elem list not empty");
		}
		mXercesNodeVector->clear();
	}

	// Raz log prefix
	//mLog.setHeader("");
}
//------------------------------------------------------------------------------

bool XmlExecRunner::IsParentToken(int inTypToken)
{
	XercesNodeVector::const_reverse_iterator it, itEnd;
	it = mXercesNodeVector->rbegin();
	itEnd = mXercesNodeVector->rend();
	for (; it != itEnd; ++it) {
		XercesNode *node = *it;
		if (node->GetTypToken() == inTypToken)
			return true;
	}

	return false;
}
//------------------------------------------------------------------------------

void XmlExecRunner::ClearDocumentInfo()
{
	mNbError = 0;

	if (mInfo->mPluginMessage)
		mInfo->mPluginMessage->mMessage.remove();
}
//------------------------------------------------------------------------------

void XmlExecRunner::StartNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap)
{
	XmlReq *xmlReq = mXmlReq.get();

	// Identify the token
	eXmlCmd xmlCmd;
	StringToIntMap::const_iterator itMap = mTokenMap.find(inElem);
	if (itMap != mTokenMap.end()) {
		xmlCmd = (eXmlCmd)itMap->second;
	}
	else {
		xmlCmd = eXmlCmdNone;
		mLog.log(eTypLogError, "Err > XmlExecRunner Xml elem unrecognized: %s", inElem.c_str());
	}

	switch (xmlCmd) {
	case eAtollXmlExec:
	case eReqAll:
	case eAction:
	case eGetListCol:
	case eGetListDoc:
	case eGetListNdx:
	case eVersion:
	case eNewSearch:
	case eTypeRes:
	case ePage:
	case ePos:
	case eNbMot:
		// Nothing to do
		break;
	case ePlage:
		mXmlReq->mMin = 0;
		mXmlReq->mNb = DEF_LongMax;
		mXmlReq->mMax = DEF_LongMax;
		break;
	case eReqCol:
		mXmlReq.reset(new XmlReqCol(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		mXmlReq->OutputHeader();
		break;
	case eReqDb:
		mXmlReq.reset(new XmlReqDb(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		mXmlReq->OutputHeader();
		break;
	case eReqInit:
		mXmlReq.reset(new XmlReqInit(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		break;
	case eReqInfo:
		mXmlReq.reset(new XmlReqInfo(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		break;
	case eReqSearch:
		mXmlReq.reset(new XmlReqSearch(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		break;
	case eReqIndex:
		mXmlReq.reset(new XmlReqIndex(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		break;
	case eReqDoc:
		mXmlReq.reset(new XmlReqDoc(mLog, *(mInfo->mPluginMessage), *(mInfo->mEngineEnv)));
		break;
	case eLogFile:
	case eTraceFile:
		if (!IsParentToken(eReqInfo)) {
			mLog.log(eTypLogWarning, "Wrn > Incorrect xml command <%s>", gXmlTok(xmlCmd));
		}
		break;
	case eOutFile:
	case eDbHome:
	case eUser:
		if (IsParentToken(eReqInit)) {
			mWantInputBuffer = true;
			mInputBuffer.remove();
		}
		break;
	case eSearchCrit:
		xmlReq->ClearSearchCrit();
		break;
	case eCrit:
		mSearchCrit->Clear();
		break;
	case eCol:
	case eAddCol:
	case eGetCol:
	case eSetCol:
	case eDelCol:
		AppAssert(mColMeta == NULL);
		mColMeta = new ColMeta;
		{
			StringToUnicodeStringMap::const_iterator it, itEnd;
			for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
				const std::string &attrName = it->first;
				if (attrName == gXmlTok(eColId)) {
					mColMeta->mColId = it->second;
				}
				else if (attrName == gXmlTok(eColTitle)) {
					mColMeta->mColTitle = it->second;
				}
				else if (attrName == gXmlTok(eColSalt)) {
					mColMeta->mColSalt = it->second;
				}
				else {
					mLog.log(eTypLogWarning, "Wrn > Incorrect col meta attribute: \"%s\"", attrName.c_str());
				}
			}
		}
		mWantInputBuffer = true;
		mInputBuffer.remove();
		break;
	case eAddDoc:
	case eGetDoc:
	case eSetDoc:
	case eDelDoc:
	case eIndexDoc:
	case eDelDocIndex:
		AppAssert(mDocMeta == NULL);
		mDocMeta = new DocMeta;
		{
			StringToUnicodeStringMap::const_iterator it, itEnd;
			for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
				const std::string &attrName = it->first;
				if (attrName == gXmlTok(eDocNum)) {
					mDocMeta->mDocNum = UnicodeStringScanULong(it->second);
				}
				else if (attrName == gXmlTok(eCountPge)) {
					mDocMeta->mCountPge = UnicodeStringScanULong(it->second);
				}
				else if (attrName == gXmlTok(eFileName)) {
					ConvertUnicodeString2String(mDocMeta->mFileName, it->second);
				}
				else if (attrName == gXmlTok(eUuid)) {
					mDocMeta->mUuid = it->second;
				}
				else if (attrName == gXmlTok(eXsltCode)) {
					mDocMeta->mXsltCode = it->second;
				}
				else if (attrName == gXmlTok(eDcMetadata)) {
					ConvertUnicodeString2String(mDocMeta->mDcMetadata, it->second);
				}
				else if (attrName == gXmlTok(eIndexerConfig)) {
					ConvertUnicodeString2String(mDocMeta->mIndexerConfig, it->second);
				}
				else if (attrName == gXmlTok(eRecordBreakerConfig)) {
					ConvertUnicodeString2String(mDocMeta->mRecordBreakerConfig, it->second);
				}
				else if (attrName == gXmlTok(eXmlValidation)) {
					bool isTrue = (it->second == "1" || it->second == "true" || it->second == "yes");
					mDocMeta->mXmlValidation = isTrue;
				}
				else if (attrName == gXmlTok(eIsIndexed)) {
					bool isTrue = (it->second == "1" || it->second == "true" || it->second == "yes");
					mDocMeta->mIsIndexed = isTrue;
				}
				else {
					mLog.log(eTypLogWarning, "Wrn > Incorrect doc meta attribute: \"%s\"", attrName.c_str());
				}
			}
		}
		mWantInputBuffer = true;
		mInputBuffer.remove();
		break;
	case eAddField:
	case eGetField:
	case eSetField:
	case eDelField:
	case eGetListField:
		AppAssert(mFieldMeta == NULL);
		mFieldMeta = new FieldMeta;
		{
			std::string value;
			StringToUnicodeStringMap::const_iterator it, itEnd;
			for (it = inAttrMap.begin(), itEnd = inAttrMap.end(); it != itEnd; ++it) {
				const std::string &attrName = it->first;
				if (attrName == gXmlTok(eType)) {
					ConvertUnicodeString2String(value, it->second);
					mFieldMeta->SetFieldType(value);
				}
				else if (attrName == gXmlTok(eFieldId)) {
					mFieldMeta->SetFieldId(it->second);
				}
				else if (attrName == gXmlTok(eOrder)) {
					mFieldMeta->mFieldOrder = UnicodeStringScanULong(it->second);
				}
				else {
					mLog.log(eTypLogWarning, "Wrn > Incorrect field meta attribute: \"%s\"", attrName.c_str());
				}
			}
		}
		mWantInputBuffer = true;
		mInputBuffer.remove();
		break;
	case eSearchId:
	case eDocPagPos:
	case eIdCrit:
	case eTexte:
	case eMin:
	case eMot:
	case eNb:
		mWantInputBuffer = true;
		mInputBuffer.remove();
		break;
	case eTypeIndex:
  case eIndex:
  case eListe:
  case eArticle:
		if (IsParentToken(eReqIndex)) {
			(static_cast<XmlReqIndex *>(xmlReq))->mTypIndex = (xmlCmd == eTypeIndex) ? eXmlCmdNone : xmlCmd;
		}
		else {
			mLog.log(eTypLogWarning, "Wrn > Incorrect xml command <%s>", gXmlTok(xmlCmd));
		}
		break;
	case eTypeTri:
	case eMotFreq:
	case eFreqCroi:
	case eFreqDecr:
	case eFinMot:
	case eNumCroi:
	case eNumDecr:
		if (IsParentToken(eReqIndex)) {
			(static_cast<XmlReqIndex *>(xmlReq))->mTypSort = (xmlCmd == eTypeTri) ? eXmlCmdNone : xmlCmd;
		}
		else {
			mLog.log(eTypLogWarning, "Wrn > Incorrect xml command <%s>", gXmlTok(xmlCmd));
		}
		break;
	default:
		if (xmlCmd != eXmlCmdNone) {
			const char *elem = gXmlTok(xmlCmd);
			mLog.log(eTypLogWarning, "Wrn > Incorrect open xml command: <%s>", elem);
		}
		break;
	}

	XercesNode *xercesNode = new XercesNode(inElem, inAttrMap, xmlCmd);
	mXercesNodeVector->push_back(xercesNode);
}
//------------------------------------------------------------------------------

void XmlExecRunner::EndNode()
{
	XercesNode *xercesNode = mXercesNodeVector->back();
	mXercesNodeVector->pop_back();
	eXmlCmd xmlCmd = (eXmlCmd)xercesNode->GetTypToken();
	XmlReq *xmlReq = mXmlReq.get();

	switch (xmlCmd) {
	case eAtollXmlExec:
	case eReqAll:
	case eAction:
	case eSearchCrit:
	case eNewSearch:
	case ePlage:
	case eTypeRes:
	case ePage:
	case ePos:
	case eNbMot:
	case eTypeIndex:
  case eIndex:
  case eListe:
	case eArticle:
	case eTypeTri:
	case eMotFreq:
	case eFreqCroi:
	case eFreqDecr:
	case eFinMot:
	case eNumCroi:
	case eNumDecr:
		// Nothing to do
		break;
	case eReqInit:
	case eReqInfo:
	case eReqSearch:
	case eReqIndex:
	case eReqDoc:
		{
			// Execute the request
			bool isOk = xmlReq->Execute();
			if (!isOk)
				mInfo->mPluginMessage->mIsOk = false;
		}
		ReleaseRequest();
		break;
	case eReqCol:
	case eReqDb:
		// Request is already executed
		mXmlReq->OutputFooter();
		ReleaseRequest();
		break;
	case eDbHome:
		if (IsParentToken(eReqInit)) {
			std::string path = UnicodeString2String(mInputBuffer);
			(static_cast<XmlReqInit *>(xmlReq))->mDbHome = path;
			mWantInputBuffer = false;
		}
		else if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsDbHome = true;
		}
		break;
	case eLogFile:
		if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsLogFile = true;
		}
		break;
	case eTraceFile:
		if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsTraceFile = true;
		}
		break;
	case eOutFile:
		if (IsParentToken(eReqInit)) {
			std::string fileName = UnicodeString2String(mInputBuffer);
			SetOutputFile(fileName);
			mWantInputBuffer = false;
		}
		else if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsOutputFile = true;
			(static_cast<XmlReqInfo *>(xmlReq))->mOutputFile = mInfo->mOutputFile;
		}
		break;
	case eUser:
		if (IsParentToken(eReqInit)) {
			(static_cast<XmlReqInit *>(xmlReq))->mUser = mInputBuffer;
			mWantInputBuffer = false;
		}
		else if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsUser = true;
		}
		break;
	case eVersion:
		if (IsParentToken(eReqInfo)) {
			(static_cast<XmlReqInfo *>(xmlReq))->mIsVersion = true;
		}
		break;
	case eGetListCol:
		(static_cast<XmlReqDb *>(xmlReq))->ListCol();
		break;
	case eGetListDoc:
		(static_cast<XmlReqCol *>(xmlReq))->ListDoc();
		break;
	case eGetListNdx:
		(static_cast<XmlReqCol *>(xmlReq))->ListNdx();
		break;
	case eCol:
	case eAddCol:
	case eGetCol:
	case eSetCol:
	case eDelCol:
		AppAssert(mColMeta != NULL);
		mColMeta->mColDesc = mInputBuffer;
		switch (xmlCmd) {
		case eCol:
			if (IsParentToken(eReqInit)) {
				(static_cast<XmlReqInit *>(xmlReq))->mColId = mColMeta->mColId;
			}
			else if (IsParentToken(eReqInfo)) {
				(static_cast<XmlReqInfo *>(xmlReq))->mIsCol = true;
			}
			break;
		case eAddCol:
			mColMeta->Trim();
			if (!(static_cast<XmlReqDb *>(xmlReq))->AddColMeta(*mColMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eGetCol:
			if (!(static_cast<XmlReqDb *>(xmlReq))->GetColMeta(*mColMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eSetCol:
			mColMeta->Trim();
			if (!(static_cast<XmlReqDb *>(xmlReq))->SetColMeta(*mColMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eDelCol:
			if (!(static_cast<XmlReqDb *>(xmlReq))->DelCol(mColMeta->mColId))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		default:
			break;
		}
		delete mColMeta;
		mColMeta = NULL;
		mWantInputBuffer = false;
		break;
	case eAddDoc:
	case eGetDoc:
	case eSetDoc:
	case eDelDoc:
	case eIndexDoc:
	case eDelDocIndex:
		AppAssert(mDocMeta != NULL);
		mDocMeta->mDocTitle = mInputBuffer;
		switch (xmlCmd) {
		case eAddDoc:
			mDocMeta->Trim();
			if (!(static_cast<XmlReqCol *>(xmlReq))->AddDocMeta(*mDocMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eGetDoc:
			if (!(static_cast<XmlReqCol *>(xmlReq))->GetDocMeta(*mDocMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eSetDoc:
			mDocMeta->Trim();
			if (!(static_cast<XmlReqCol *>(xmlReq))->SetDocMeta(*mDocMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eDelDoc:
			if (!(static_cast<XmlReqCol *>(xmlReq))->DelDoc(mDocMeta->mDocNum))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eIndexDoc:
			if (!(static_cast<XmlReqCol *>(xmlReq))->IndexDoc(mDocMeta->mDocNum))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eDelDocIndex:
			if (!(static_cast<XmlReqCol *>(xmlReq))->DelDocIndex(mDocMeta->mDocNum))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		default:
			break;
		}
		delete mDocMeta;
		mDocMeta = NULL;
		mWantInputBuffer = false;
		break;
	case eAddField:
	case eGetField:
	case eSetField:
	case eDelField:
	case eGetListField:
		AppAssert(mFieldMeta != NULL);
		mFieldMeta->mFieldTitle = mInputBuffer;
		switch (xmlCmd) {
		case eAddField:
			mFieldMeta->Trim();
			if (!(static_cast<XmlReqCol *>(xmlReq))->AddFieldMeta(*mFieldMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eGetField:
			if (!(static_cast<XmlReqCol *>(xmlReq))->GetFieldMeta(*mFieldMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eSetField:
			mFieldMeta->Trim();
			if (!(static_cast<XmlReqCol *>(xmlReq))->SetFieldMeta(*mFieldMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eDelField:
			if (!(static_cast<XmlReqCol *>(xmlReq))->DelFieldMeta(*mFieldMeta))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		case eGetListField:
			if (!(static_cast<XmlReqCol *>(xmlReq))->ListField(mFieldMeta->mTypField))
				mInfo->mPluginMessage->mIsOk = false;
			break;
		default:
			break;
		}
		delete mFieldMeta;
		mFieldMeta = NULL;
		mWantInputBuffer = false;
		break;
	case eCrit:
		// <Crit><IdCrit>106</IdCrit><Texte>femme</Texte></Crit>
		xmlReq->AddSearchCrit(*mSearchCrit);
		break;
	case eSearchId:
		mWantInputBuffer = false;
		xmlReq->SetSearchId(UnicodeString2String(mInputBuffer));
		break;
	case eDocPagPos:
		mWantInputBuffer = false;
		{
			Entry entry;
			std::string strDpp = UnicodeString2String(mInputBuffer);
			if (!entry.FromStringDocPagePos(strDpp.c_str())) {
				mNbError++;
				break;
			}
			xmlReq->mDoc = entry.mUiEntDoc;
			xmlReq->mPge = entry.mUlEntPge;
			xmlReq->mPos = entry.mUlEntPos;
		}
		// Set the action to perform
		switch (xmlReq->GetXmlCmdReq()) {
		case eReqDoc:
			(static_cast<XmlReqDoc *>(xmlReq))->mAction = xmlCmd;
			break;
		case eReqUsr:
			{
				Entry entry(xmlReq->mDoc, xmlReq->mPge, xmlReq->mPos);
				//((XmlReqUsr *)theExe)->mMlEntry->Append(entry);
			}
			break;
		default:
			{
				const char *elem = gXmlTok(xmlCmd);
				mLog.log(eTypLogWarning, "Wrn > Incorrect xml command inside <DocPagPos>: </%s>", elem);
			}
			break;
		}
		break;
	case eIdCrit:
		{
			//UTrace("Set IdCrit", mInputBuffer.getTerminatedBuffer());
			if (IsParentToken(eReqIndex)) {
				(static_cast<XmlReqIndex *>(xmlReq))->SetIndexCrit(mInputBuffer);
			}
			else {
				mSearchCrit->SetIdCrit(mInputBuffer);
			}
			mWantInputBuffer = false;
		}
		break;
	case eTexte:
		if (IsParentToken(eCrit))
			mSearchCrit->mWrd = mInputBuffer;
		else
			mLog.log(eTypLogError, "Err > No text content handler");
		mWantInputBuffer = false;
		break;
	case eMin:
		xmlReq->mMin = UnicodeStringScanULong(mInputBuffer);
		break;
	case eMot:
		if (IsParentToken(eReqIndex)) {
			(static_cast<XmlReqIndex *>(xmlReq))->mWord = mInputBuffer;
		}
		break;
	case eNb:
		xmlReq->mNb = UnicodeStringScanULong(mInputBuffer);
		break;
	default:
		if (xmlCmd != eXmlCmdNone) {
			const char *elem = gXmlTok(xmlCmd);
			mLog.log(eTypLogWarning, "Wrn > Incorrect close xml command: </%s>", elem);
		}
		break;
	}

	delete xercesNode;
}
//------------------------------------------------------------------------------

void XmlExecRunner::AddCharacters(const UChar *inStr, unsigned int inLength)
{
	if (!mWantInputBuffer)
		return;

	mInputBuffer.append(inStr, inLength);
}
//------------------------------------------------------------------------------

void XmlExecRunner::OutputText(const char *inStr)
{
	// Add data in the message
	mInfo->mPluginMessage->mMessage += inStr;
}
//------------------------------------------------------------------------------

void XmlExecRunner::OutputText(UnicodeString &inStr)
{
	// Add data in the message
	mInfo->mPluginMessage->mMessage += inStr;
}
//------------------------------------------------------------------------------

void XmlExecRunner::ReleaseRequest()
{
	//XmlReq *xmlReq = mXmlReq.get();
	//xmlReq->Execute();
	delete mXmlReq.release();
}
//------------------------------------------------------------------------------
