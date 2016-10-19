/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecCmd.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XmlExecCmd.hpp"
//------------------------------------------------------------------------------

using namespace AtollPluginXmlExec;

XmlExecCmd XmlExecCmd::mHandle; // Base object for the map
const char *XmlExecCmd::mXmlCmd[];
bool XmlExecCmd::mIsFirstAccess = true;
XmlExecCmd::AssocMap XmlExecCmd::mStaticMap;

XmlExecCmd::AssocMap &XmlExecCmd::StaticGetMap()
{
	if (!mIsFirstAccess) {
		return mStaticMap;
	}
	mIsFirstAccess = false;

	// Begin
	mStaticMap["XmlCmdNone"] = eXmlCmdNone;
	mStaticMap["atoll-plugin-xmlexec"] = eAtollXmlExec;
	// Requests
	mStaticMap["ReqAll"] = eReqAll;
	mStaticMap["ResAll"] = eResAll;
	mStaticMap["ReqInit"] = eReqInit;
	mStaticMap["ResInit"] = eResInit;
	mStaticMap["ReqInfo"] = eReqInfo;
	mStaticMap["ResInfo"] = eResInfo;
	mStaticMap["ReqUtil"] = eReqUtil;
	mStaticMap["ResUtil"] = eResUtil;
	mStaticMap["ReqDb"] = eReqDb;
	mStaticMap["ResDb"] = eResDb;
	mStaticMap["ReqCol"] = eReqCol;
	mStaticMap["ResCol"] = eResCol;
	mStaticMap["ReqDoc"] = eReqDoc;
	mStaticMap["ResDoc"] = eResDoc;
	mStaticMap["ReqSearch"] = eReqSearch;
	mStaticMap["ResSearch"] = eResSearch;
	mStaticMap["ReqIndex"] = eReqIndex;
	mStaticMap["ResIndex"] = eResIndex;
	mStaticMap["ReqSearchCrit"] = eReqSearchCrit;
	mStaticMap["ResSearchCrit"] = eResSearchCrit;
	mStaticMap["ReqSearchScreen"] = eReqSearchScreen;
	mStaticMap["ResSearchScreen"] = eResSearchScreen;
	mStaticMap["ReqUsr"] = eReqUsr;
	mStaticMap["ResUsr"] = eResUsr;
	mStaticMap["ResError"] = eResError;
	// Commands
	mStaticMap["Version"] = eVersion;
	mStaticMap["LogFile"] = eLogFile;
	mStaticMap["TraceFile"] = eTraceFile;
	mStaticMap["OutFile"] = eOutFile;
	mStaticMap["DbHome"] = eDbHome;
	mStaticMap["ColPath"] = eColPath;
	mStaticMap["UsrPath"] = eUsrPath;
	mStaticMap["FacPath"] = eFacPath;
	mStaticMap["SrvPath"] = eSrvPath;
	mStaticMap["UsrDynPath"] = eUsrDynPath;
	mStaticMap["UsrHstPath"] = eUsrHstPath;
	mStaticMap["UsrReqPath"] = eUsrReqPath;
	mStaticMap["AttGraphie"] = eAttGraphie;
	mStaticMap["AttLang"] = eAttLang;
	mStaticMap["AttTypClient"] = eAttTypClient;
	mStaticMap["AttTypOs"] = eAttTypOs;
	mStaticMap["AttUsrAction"] = eAttUsrAction;
	mStaticMap["AttUsrActionAnnoGet"] = eAttUsrActionAnnoGet;
	mStaticMap["AttUsrActionAnnoSet"] = eAttUsrActionAnnoSet;
	mStaticMap["AttUsrActionAnnoDel"] = eAttUsrActionAnnoDel;
	mStaticMap["AttUsrActionAnnoList"] = eAttUsrActionAnnoList;
	mStaticMap["AttUsrActionSignetGet"] = eAttUsrActionSignetGet;
	mStaticMap["AttUsrActionSignetSet"] = eAttUsrActionSignetSet;
	mStaticMap["AttUsrActionSignetDel"] = eAttUsrActionSignetDel;
	mStaticMap["AttUsrActionSignetList"] = eAttUsrActionSignetList;
	mStaticMap["AttUsrActionSearchCritGet"] = eAttUsrActionSearchCritGet;
	mStaticMap["AttUsrActionSearchCritSet"] = eAttUsrActionSearchCritSet;
	mStaticMap["AttUsrActionSearchCritDel"] = eAttUsrActionSearchCritDel;
	mStaticMap["AttUsrActionSearchCritList"] = eAttUsrActionSearchCritList;
	mStaticMap["AttUsrActionHistRechGet"] = eAttUsrActionHistRechGet;
	mStaticMap["AttUsrActionHistRechSet"] = eAttUsrActionHistRechSet;
	mStaticMap["AttUsrActionHistRechDel"] = eAttUsrActionHistRechDel;
	mStaticMap["AttUsrActionHistRechList"] = eAttUsrActionHistRechList;
	mStaticMap["AttUsrActionProfilGet"] = eAttUsrActionProfilGet;
	mStaticMap["AttUsrActionProfilSet"] = eAttUsrActionProfilSet;
	mStaticMap["AttUsrActionProfilDel"] = eAttUsrActionProfilDel;
	mStaticMap["AttUsrActionProfilList"] = eAttUsrActionProfilList;
	mStaticMap["AttUsrActionCorpusNew"] = eAttUsrActionCorpusNew;
	mStaticMap["AttUsrActionCorpusDel"] = eAttUsrActionCorpusDel;
	mStaticMap["AttUsrActionCorpusRename"] = eAttUsrActionCorpusRename;
	mStaticMap["AttUsrActionCorpusClear"] = eAttUsrActionCorpusClear;
	mStaticMap["AttUsrActionCorpusAddPage"] = eAttUsrActionCorpusAddPage;
	mStaticMap["AttUsrActionCorpusAddResRech"] = eAttUsrActionCorpusAddResRech;
	mStaticMap["AttUsrActionCorpusList"] = eAttUsrActionCorpusList;
	mStaticMap["AttUsrActionResRechSuppr"] = eAttUsrActionResRechSuppr;
	mStaticMap["User"] = eUser;
	mStaticMap["SearchId"] = eSearchId;
	mStaticMap["Col"] = eCol;
	mStaticMap["ColId"] = eColId;
	mStaticMap["ColTitle"] = eColTitle;
	mStaticMap["ColSalt"] = eColSalt;
	//mStaticMap["DocName"] = eDocName;
	mStaticMap["DocNum"] = eDocNum;
	mStaticMap["Source"] = eSource;
	mStaticMap["CountPge"] = eCountPge;
	mStaticMap["FileName"] = eFileName;
	mStaticMap["DcMetadata"] = eDcMetadata;
	mStaticMap["IndexerConfig"] = eIndexerConfig;
	mStaticMap["RecordBreakerConfig"] = eRecordBreakerConfig;
	mStaticMap["Uuid"] = eUuid;
	mStaticMap["XsltCode"] = eXsltCode;
	mStaticMap["XmlValidation"] = eXmlValidation;
	mStaticMap["IsIndexed"] = eIsIndexed;
	mStaticMap["Name"] = eName;
	mStaticMap["Num"] = eNum;
	mStaticMap["DocTexte"] = eDocTexte;
	mStaticMap["DocPagPos"] = eDocPagPos;
	mStaticMap["Anno"] = eAnno;
	mStaticMap["Signet"] = eSignet;
	mStaticMap["NoticeAuteur"] = eNoticeAuteur;
	mStaticMap["NoticeEditeur"] = eNoticeEditeur;
	mStaticMap["Info4emeCouv"] = eInfo4emeCouv;
	mStaticMap["Facsim"] = eFacsim;
	mStaticMap["OcrInfo"] = eOcrInfo;
	mStaticMap["Link"] = eLink;
	mStaticMap["Search"] = eSearch;
	mStaticMap["Texte"] = eTexte;
	mStaticMap["Action"] = eAction;
	mStaticMap["Open"] = eOpen;
	mStaticMap["Close"] = eClose;
	mStaticMap["Export"] = eExport;
	mStaticMap["Contexte"] = eContexte;
	mStaticMap["Def"] = eDef;
	mStaticMap["ClearCacheCol"] = eClearCacheCol;
	mStaticMap["ClearCacheUsr"] = eClearCacheUsr;
	mStaticMap["ClearCacheUsrDyn"] = eClearCacheUsrDyn;
	mStaticMap["ClearCacheUsrHst"] = eClearCacheUsrHst;
	mStaticMap["ClearCacheUsrReq"] = eClearCacheUsrReq;
	mStaticMap["StartIndex"] = eStartIndex;
	mStaticMap["UpdateIndex"] = eUpdateIndex;
	mStaticMap["MakeIndexDyn"] = eMakeIndexDyn;
	mStaticMap["MakeTabmat"] = eMakeTabmat;
	mStaticMap["GetListCol"] = eGetListCol;
	mStaticMap["ListCol"] = eListCol;
	mStaticMap["GetListDoc"] = eGetListDoc;
	mStaticMap["GetListField"] = eGetListField;
	mStaticMap["ListDoc"] = eListDoc;
	mStaticMap["ListField"] = eListField;
	mStaticMap["GetListNdx"] = eGetListNdx;
	mStaticMap["ListNdx"] = eListNdx;
	mStaticMap["ListTexte"] = eListTexte;
	mStaticMap["ListDocTexte"] = eListDocTexte;
	mStaticMap["AddCol"] = eAddCol;
	mStaticMap["GetCol"] = eGetCol;
	mStaticMap["SetCol"] = eSetCol;
	mStaticMap["DelCol"] = eDelCol;
	mStaticMap["AddDoc"] = eAddDoc;
	mStaticMap["GetDoc"] = eGetDoc;
	mStaticMap["SetDoc"] = eSetDoc;
	mStaticMap["DelDoc"] = eDelDoc;
	mStaticMap["Field"] = eField;
	mStaticMap["FieldId"] = eFieldId;
	mStaticMap["AddField"] = eAddField;
	mStaticMap["GetField"] = eGetField;
	mStaticMap["SetField"] = eSetField;
	mStaticMap["DelField"] = eDelField;
	mStaticMap["IndexDoc"] = eIndexDoc;
	mStaticMap["DelDocIndex"] = eDelDocIndex;
	mStaticMap["GetStatus"] = eGetStatus;
	mStaticMap["Status"] = eStatus;
	mStaticMap["NoIndex"] = eNoIndex;
	mStaticMap["Plage"] = ePlage;
	mStaticMap["Mot"] = eMot;
	mStaticMap["Min"] = eMin;
	mStaticMap["Nb"] = eNb;
	mStaticMap["Max"] = eMax;
	mStaticMap["TypeRes"] = eTypeRes;
	mStaticMap["Doc"] = eDoc;
	mStaticMap["Page"] = ePage;
	mStaticMap["Pos"] = ePos;
	mStaticMap["SearchCrit"] = eSearchCrit;
	mStaticMap["Crit"] = eCrit;
	mStaticMap["SortList"] = eSortList;
	mStaticMap["IdCrit"] = eIdCrit;
	mStaticMap["SearchScreen"] = eSearchScreen;
	mStaticMap["Mode"] = eMode;
	mStaticMap["OpeRech"] = eOpeRech;
	mStaticMap["TypeIndex"] = eTypeIndex;
	mStaticMap["Index"] = eIndex;
	mStaticMap["Liste"] = eListe;
	mStaticMap["Article"] = eArticle;
	mStaticMap["TypeTri"] = eTypeTri;
	mStaticMap["MotFreq"] = eMotFreq;
	mStaticMap["FreqCroi"] = eFreqCroi;
	mStaticMap["FreqDecr"] = eFreqDecr;
	mStaticMap["FinMot"] = eFinMot;
	mStaticMap["NumCroi"] = eNumCroi;
	mStaticMap["NumDecr"] = eNumDecr;
	mStaticMap["Para"] = ePara;
	mStaticMap["Phrase"] = ePhrase;
	mStaticMap["Compact"] = eCompact;
	mStaticMap["NbMot"] = eNbMot;
	mStaticMap["File"] = eFile;
	mStaticMap["Tabmat"] = eTabmat;
	mStaticMap["Tag"] = eTag;
	mStaticMap["Grec"] = eGrec;
	mStaticMap["NbRes"] = eNbRes;
	mStaticMap["NbDoc"] = eNbDoc;
	mStaticMap["NbPage"] = eNbPage;
	mStaticMap["NbPos"] = eNbPos;
	mStaticMap["FormatXml"] = eFormatXml;
	mStaticMap["FormatHtml"] = eFormatHtml;
	mStaticMap["Tool"] = eTool;
	mStaticMap["Stat"] = eStat;
	mStaticMap["Binary"] = eBinary;
	mStaticMap["UserInfo"] = eUserInfo;
	mStaticMap["Param"] = eParam;
	mStaticMap["Result"] = eResult;
	mStaticMap["ToolBar"] = eToolBar;
	mStaticMap["Object"] = eObject;
	mStaticMap["Type"] = eType;
	mStaticMap["Order"] = eOrder;
	mStaticMap["Enabled"] = eEnabled;
	mStaticMap["Checked"] = eChecked;
	mStaticMap["Corpus"] = eCorpus;
	mStaticMap["NewSearch"] = eNewSearch;
	mStaticMap["Title"] = eTitle;
	mStaticMap["SetOfFiles"] = eSetOfFiles;
	mStaticMap["BinFile"] = eBinFile;
	mStaticMap["Base64"] = eBase64;
	// End
	mStaticMap["XmlCmdEnd"] = eXmlCmdEnd;

	AssocMap::const_iterator it, itEnd;
	it = mStaticMap.begin();
	itEnd = mStaticMap.end();
  for (; it != itEnd; ++it) {
		mXmlCmd[it->second] = it->first.c_str();
	}

	if (mStaticMap.size() != eXmlCmdEnd + 1) {
		char error[255];
		sprintf(error, "Xml map size error: Map=%u - Xml=%d", (unsigned int)mStaticMap.size(), eXmlCmdEnd + 1);
		throw Common::AppException(Common::AppException::InvalidValue, error);
	}

	return mStaticMap;
}
//------------------------------------------------------------------------------
